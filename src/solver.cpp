/** 
 * @file solver.cpp
 * @brief Solver class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 19.07.2009
 */

#include <fstream>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "model_smv.hpp"
#include "counterexample.hpp"
#include "solver.hpp"

using std::string;
using std::fstream;
using std::ios;
using std::logic_error;
using std::cout;
using std::endl;

namespace mc_hybrid
{
  /** 
   * @brief Fills fm system from constraints group.
   * 
   * @param[in] problem Problem.
   * @param[in] group   Constraints group.
   * 
   * @return Filled fm system.
   */
  s_fm_system_t*
  fill_fm_system(Problem* problem,
                 Problem::Constrs_group group,
                 bool include_vars_ranges)
  {
    size_t constraints_num = problem->get_constraints_num(group);
    size_t variables_num = problem->get_constraints_vars_num(group);

    size_t vars_ranges_count = include_vars_ranges ? variables_num * 2 : 0;
    
    // number of lines - number of constraints in group +
    // 2 lines for each variable range
    // number of columns - number of variable
    // + 1 for constraint type
    // + 1 for free_member
    s_fm_system_t* system = fm_system_alloc(constraints_num + 
                                            vars_ranges_count,
                                            variables_num + 2);
    // adding lines for constraints
    for (size_t i = 0; i < constraints_num; ++i)
    {
      Constraint& c = problem->get_constraint(group, i);
      s_fm_vector_t* line = system->lines[i];
      if (c.get_type() == Constraint::EQUAL)
        fm_vector_set_eq(line);
      else
        fm_vector_set_ineq(line);
      for (size_t j = 0; j < variables_num; ++j)
      {
        Variable& v = problem->get_constraints_var(group, j);
        s_fm_rational_t* coeff = real_t_to_fm_rational(c.get_coeff(v));
        fm_vector_assign_idx(line, coeff, j + 1);
        fm_rational_free(coeff);
      }
      s_fm_rational_t* free_member = real_t_to_fm_rational(c.get_free_member());
      fm_vector_assign_idx(line, free_member, variables_num + 1);
      fm_rational_free(free_member);
    }

    // adding lines for variables ranges
    if (include_vars_ranges == true)
    {
      for (size_t i = 0; i < variables_num; ++i)
      {
        s_fm_vector_t* line;
        s_fm_rational_t* free_member;
        Variable& v = problem->get_constraints_var(group, i);
        // adding line for lower bound
        line = system->lines[constraints_num + i * 2];
        fm_vector_set_ineq(line);
        fm_vector_assign_int_idx(line, int_t(1).get_mpz_t(), i + 1);
        free_member = real_t_to_fm_rational(-v.get_lower_bound());
        fm_vector_assign_idx(line, free_member, variables_num + 1);
        fm_rational_free(free_member);
        // adding line for upper bound
        line = system->lines[constraints_num + i * 2 + 1];
        fm_vector_set_ineq(line);
        fm_vector_assign_int_idx(line, int_t(-1).get_mpz_t(), i + 1);
        free_member = real_t_to_fm_rational(v.get_upper_bound());
        fm_vector_assign_idx(line, free_member, variables_num + 1);
        fm_rational_free(free_member);
      }
    }

    return system;
  }

  /** 
   * @brief Eliminates variables from constraints group.
   * 
   * @param[in]  group               Constraints group.
   * @param[in]  vars_num            Number of variables to eliminate.
   * @param[in]  problem_source      Source problem
   * @param[out] problem_destination Destination problem.
   */
  void
  eliminate_variables(Problem::Constrs_group group,
                      size_t vars_num,
                      bool include_vars_ranges,
                      Problem* problem_source,
                      Problem* problem_destination)
  {
    if (vars_num > 0)
    {
      size_t variables_num = problem_source->get_constraints_vars_num(group);
      size_t vars_to_num = variables_num - vars_num;
      s_fm_system_t* system = fill_fm_system(problem_source,
                                             group,
                                             include_vars_ranges);
      s_fm_solution_t* solution = fm_solver_solution_to(system, 0, vars_to_num);
      fm_system_free(system);
      if (solution->size > 0)
      {
        s_fm_system_t* system_new = fm_solution_to_system(solution);
        for (size_t j = 0; j < system_new->nb_lines; ++j)
        {
          Constraint c;
          s_fm_vector_t* line = system_new->lines[j];
          // first idx - constraint type
          if (line->vector[0].num == 0)
            c.set_type(Constraint::EQUAL);
          else
            c.set_type(Constraint::MORE_OR_EQUAL);
          for (size_t k = 1; k < line->size - 1; ++k)
          {
            Variable& v = problem_source->get_constraints_var(group, k - 1);
            c.set_coeff(v, fm_rational_to_real_t(&line->vector[k]));
          }
          // last idx - free_member
          c.set_free_member(fm_rational_to_real_t(&line->vector[line->size - 1]));
          //c.make_coeffs_integer();
          problem_destination->add_constraint(group, c);
        }
        fm_system_free(system_new);
      }
      fm_solution_free(solution);
    }
    else
    {
      for (size_t i = 0; i < problem_source->get_constraints_num(group); ++i)
      {
        Constraint& c = problem_source->get_constraint(group, i);
        problem_destination->add_constraint(group, c);
      }
    }
  }

  Solver::Solver()
  {
    problem_original = 0;
    problem_without_outputs = 0;
    problem_quantized = 0;
    problem_discrete = 0;
    problem_pb = 0;
    
    model_smv = 0;

    counterexample_boolean = 0;
    counterexample_discrete = 0;
    counterexample_mixed = 0;

    q_params.clear();
  }
  
  Solver::~Solver()
  {
    if (problem_original)
      delete problem_original;
    problem_original = 0;

    if (problem_without_outputs)
      delete problem_without_outputs;
    problem_without_outputs = 0;

    if (problem_quantized)
      delete problem_quantized;
    problem_quantized = 0;

    if (problem_discrete)
      delete problem_discrete;
    problem_discrete = 0;

    if (problem_pb)
      delete problem_pb;
    problem_pb = 0;

    if (model_smv)
      delete model_smv;
    model_smv = 0;

    if (counterexample_boolean)
      delete counterexample_boolean;
    counterexample_boolean = 0;

    if (counterexample_discrete)
      delete counterexample_discrete;
    counterexample_discrete = 0;

    if (counterexample_mixed)
      delete counterexample_mixed;
    counterexample_mixed = 0;
  }  

  ulong_t
  Solver::verify(const string& filename, int initial_q_param, int /*verbosity*/)
  {
    timeval time_start;
    gettimeofday(&time_start, NULL);

    fstream file;
    file.open(filename.c_str(), ios::in);
    problem_original = new Problem(file);
    file.close();

    make_problem_without_outputs(); 

    for (size_t i = 0; i < problem_without_outputs->get_variables_num(); ++i)
    {
      Variable& v = problem_without_outputs->get_variable(i);
      if (v.get_type() == Variable::REAL)
        q_params.insert(make_pair(v.get_name(), initial_q_param));
    }
    make_problem_quantized();
    make_problem_discrete();
    //bool stop = false;
    //while (stop != true)
    //{
      //make_problem_quantized();
      //make_problem_discrete();
      //make_problem_pb();
      //make_model_smv();
      //while (true)
      //{
        //if (verify_model_smv() == false) // If SMV model verification fails.
        //{
          //if (check_counterexample() == true) // If counterexample is true.
          //{
            //result = false; // Result is false.
            //stop = true;    // Stop verification process.
            //break;
          //}
          //if (refine_model_smv() == false) // If refinement fails.
          //{
            //change_q_params();
            //break;
          //}
        //}
        //else // If SMV model verification succeeds.
        //{
          //result = true; // Result is true.
          //stop = true;   // Stop verification process.
          //break;
        //}
      //}
    //}

    timeval time_finish;
    gettimeofday(&time_finish, NULL);

    return timeval_to_ms(time_finish) - timeval_to_ms(time_start);
  }
  
  const Counterexample*
  Solver::get_counterexample() const
  {
    return counterexample_mixed;
  }

  void
  Solver::make_problem_without_outputs()
  {
    if (problem_original == 0)
      throw logic_error("Original problem doesn't exist.");

    if (problem_without_outputs != 0)
      delete problem_without_outputs;

    problem_without_outputs = new Problem();

    for (size_t i = 0; i < Problem::VARS_GROUPS_TOTAL; ++i)
    {
      Problem::Vars_group group = Problem::Vars_group(i);
      if (group != Problem::VARS_OUTPUT)
      {
        size_t vars_num = problem_original->get_variables_num(group);
        for (size_t j = 0; j < vars_num; ++j)
        {
          Variable& v = problem_original->get_variable(group, j); 
          problem_without_outputs->add_variable(group, v);
        }
      }
    }

    for (size_t i = 0; i < Problem::CONSTRS_GROUPS_TOTAL; ++i)
    {
      Problem::Constrs_group group = Problem::Constrs_group(i);
      if (group == Problem::CONSTRS_TRANS)
        eliminate_variables(group,
                            problem_original->get_variables_num(Problem::VARS_OUTPUT),
                            true,
                            problem_original,
                            problem_without_outputs);
      else
      {
        size_t constrs_num = problem_original->get_constraints_num(group);
        for (size_t j = 0; j < constrs_num; ++j)
        {
          Constraint& c = problem_original->get_constraint(group, j);
          problem_without_outputs->add_constraint(group, c);
        }
      }
    }
  }

  void
  Solver::make_problem_quantized()
  {
    if (problem_without_outputs == 0)
      throw logic_error("Problem without outputs doesn't exist.");

    if (problem_quantized)
      delete problem_quantized;

    problem_quantized = new Problem(*problem_without_outputs);
    
    for (size_t i = 0; i < Problem::VARS_GROUPS_TOTAL; ++i)
    {
      Problem::Vars_group group = Problem::Vars_group(i);
      for (size_t j = 0; j < problem_without_outputs->get_variables_num(group); ++j)
      {
        if (problem_quantized->get_variable(group, j).get_type() == Variable::REAL)
        {
          string var_name = problem_quantized->get_variable(group, j).get_name();
          real_t lower_bound =
            problem_quantized->get_variable(group, j).get_lower_bound();
          real_t upper_bound =
            problem_quantized->get_variable(group, j).get_upper_bound();
          size_t q_param = q_params[var_name];
          real_t range = upper_bound - lower_bound;
          real_t delta = range / q_param;
          string delta_var_name = "delta_" + var_name;
          Variable delta_var(delta_var_name, Variable::REAL, 0, delta);
          problem_quantized->add_variable(group, delta_var);
          for (size_t k = 0; k < problem_quantized->get_constraints_num(); ++k)
          {
            Constraint& c = problem_quantized->get_constraint(k);
            real_t coeff = c.get_coeff(var_name);
            c.set_coeff(delta_var_name, coeff);
            real_t free_member = c.get_free_member();
            free_member += coeff * lower_bound;
            c.set_free_member(free_member);
            coeff *= delta;
            c.set_coeff(var_name, coeff);
          }
          problem_quantized->get_variable(group, j).set_type(Variable::INTEGER);
          problem_quantized->get_variable(group, j).set_lower_bound(0);
          problem_quantized->get_variable(group, j).set_upper_bound(q_param - 1);
        }
      }
    }
  }

  void
  Solver::make_problem_discrete()
  {
    if (problem_quantized == 0)
      throw logic_error("Quantized problem doesn't exist.");

    if (problem_discrete != 0)
      delete problem_discrete;

    problem_discrete = new Problem();

    // adding variables
    for (size_t i = 0; i < Problem::VARS_GROUPS_TOTAL; ++i)
    {
      Problem::Vars_group group = Problem::Vars_group(i);
      for (size_t j = 0; j < problem_quantized->get_variables_num(group); ++j)
      {
        Variable& v = problem_quantized->get_variable(group, j);
        if (v.get_type() == Variable::INTEGER)
          problem_discrete->add_variable(group, v);
      }
    }

    // adding constraints
    for (size_t i = 0; i < Problem::CONSTRS_GROUPS_TOTAL; ++i)
    {
      Problem::Constrs_group group = Problem::Constrs_group(i);
      size_t real_vars_num = 0;
      size_t vars_num = problem_quantized->get_constraints_vars_num(group);
      for (size_t j = 0; j < vars_num; ++j)
      {
        Variable& v = problem_quantized->get_constraints_var(group, j);
        if (v.get_type() == Variable::REAL)
          ++real_vars_num;
      }
      eliminate_variables(group,
                          real_vars_num,
                          true,
                          problem_quantized,
                          problem_discrete);
    }
  }

  void
  Solver::make_problem_pb()
  {
  }

  void
  Solver::make_model_smv()
  {
  }

  bool
  Solver::verify_model_smv()
  {
    // TODO: implement
    return false;
  }

  bool
  Solver::check_counterexample()
  {
    // TODO: implement
    return false;
  }

  bool
  Solver::refine_model_smv()
  {
    // TODO: implement
    return false;
  }

  void
  Solver::change_q_params()
  {
  }
}; // namespace mc_hybrid
