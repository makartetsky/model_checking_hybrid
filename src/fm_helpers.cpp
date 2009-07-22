/**
 * @file fm_helpers.cpp
 * @brief Helper functions for using FM library definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "fm_helpers.hpp"

namespace mc_hybrid
{
  s_fm_system_t*
  fill_fm_system(Problem* problem,
                 Problem::Constrs_group group)
  {
    size_t constraints_num = problem->get_constraints_num(group);
    size_t variables_num = problem->get_constraints_vars_num(group);

    size_t vars_ranges_count = variables_num * 2;

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

    return system;
  }

  void
  eliminate_variables(Problem::Constrs_group group,
                      size_t vars_num,
                      Problem* problem_source,
                      Problem* problem_destination)
  {
    if (vars_num > 0)
    {
      size_t variables_num = problem_source->get_constraints_vars_num(group);
      size_t vars_to_num = variables_num - vars_num;
      s_fm_system_t* system = fill_fm_system(problem_source, group);
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

}; // namespace mc_hybrid
