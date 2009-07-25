/**
 * @file solver.cpp
 * @brief Solver class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 19.07.2009
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "model_smv.hpp"
#include "counterexample.hpp"
#include "solver.hpp"
#include "parser.hpp"
#include "fm_helpers.hpp"
#include "minisat_helpers.hpp"

using std::string;
using std::fstream;
using std::ios;
using std::logic_error;
using std::cout;
using std::endl;
using std::ostringstream;
using std::vector;

namespace mc_hybrid
{
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

    if (problem_original != 0)
      delete problem_original;
    problem_original = new Problem();
    Parser* parser = new Parser();
    parser->read(filename, *problem_original);
    delete parser;

    make_problem_without_outputs();

    for (size_t i = 0; i < problem_without_outputs->get_variables_num(); ++i)
    {
      Variable& v = problem_without_outputs->get_variable(i);
      if (v.get_type() == Variable::REAL)
        q_params.insert(make_pair(v.get_name(), initial_q_param));
    }
    make_problem_quantized();
    make_problem_discrete();
    make_problem_pb();
    make_model_smv();
    verify_model_smv();
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

    // add variables
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

    // add constraints
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
                          problem_quantized,
                          problem_discrete);
      for (size_t j = 0; j < problem_discrete->get_constraints_num(group); ++j)
      {
        Constraint& c = problem_discrete->get_constraint(group, j);
        vector<int_t> nums;
        for (size_t k = 0; k < problem_discrete->get_constraints_vars_num(group); ++k)
        {
          Variable& v = problem_discrete->get_constraints_var(group, k);
          real_t coeff = c.get_coeff(v);
          if (coeff != 0)
            nums.push_back(coeff.get_den());
        }
        real_t free_member = c.get_free_member();
        if (free_member != 0)
          nums.push_back(free_member.get_den());
        int_t lcf_num = lcf(nums);
        c.mult(lcf_num);
      }
    }
  }

  void
  Solver::make_problem_pb()
  {
    if (problem_discrete == 0)
      throw logic_error("Discrete problem doesn't exist.");

    if (problem_pb != 0)
      delete problem_pb;

    problem_pb = new Problem();

    // clear variables mapping
    vars_mapping.clear();

    // add variables
    for (size_t i = 0, index = 0; i < Problem::VARS_GROUPS_TOTAL; ++i)
    {
      Problem::Vars_group group = Problem::Vars_group(i);
      for (size_t j = 0; j < problem_discrete->get_variables_num(group); ++j)
      {
        Variable& v_d = problem_discrete->get_variable(group, j);
        real_t lower_bound = v_d.get_lower_bound();
        real_t upper_bound = v_d.get_upper_bound();
        size_t num = size_t(floor(log(upper_bound.get_d() -
                                      lower_bound.get_d()) /
                                  log(2)) +
                            1);

        for (size_t k = 0; k < num; ++k, ++index)
        {
          ostringstream stream;
          stream << k;
          string name;

          if (group == Problem::VARS_NEXT_STATE)
          {
            name = v_d.get_name();
            name.erase(name.end() - 1);
            name += "_" + stream.str() + "'";
          }
          else
            name = v_d.get_name() + "_" + stream.str();

          Variable v_b(name, Variable::INTEGER, 0, 1);
          problem_pb->add_variable(group, v_b);
          vars_mapping[v_d.get_name()].push_back(name);
        }
      }
    }

    // add constraints
    for (size_t i = 0; i < Problem::CONSTRS_GROUPS_TOTAL; ++i)
    {
      Problem::Constrs_group group = Problem::Constrs_group(i);
      for (size_t j = 0; j < problem_discrete->get_constraints_num(group); ++j)
      {
        Constraint& c_d = problem_discrete->get_constraint(group, j);
        Constraint c_b(c_d.get_type());
        real_t free_member = 0;
        for (size_t k = 0; k < problem_discrete->get_constraints_vars_num(group); ++k)
        {
          Variable& v_d = problem_discrete->get_constraints_var(group, k);
          real_t coeff = c_d.get_coeff(v_d);
          if (coeff != 0)
          {
            for (size_t w = 0; w < vars_mapping[v_d.get_name()].size(); ++w)
            {
              string& name = vars_mapping[v_d.get_name()][w];
              c_b.set_coeff(name, coeff * pow(2.0f, w));
            }
            free_member += coeff * v_d.get_lower_bound();
          }
        }
        free_member += c_d.get_free_member();
        c_b.set_free_member(free_member);
        problem_pb->add_constraint(group, c_b);
      }
    }
  }

  void
  Solver::make_model_smv()
  {
    if (problem_pb == 0)
      throw logic_error("Pseudoboolean problem doesn't exist.");

    if (model_smv != 0)
      delete model_smv;

    model_smv = new Model_smv(problem_pb);
  }

  bool
  Solver::verify_model_smv()
  {
    if (model_smv == 0)
      throw logic_error("SMV model doesn't exist.");

    return model_smv->verify();
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
