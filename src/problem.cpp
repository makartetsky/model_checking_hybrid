/**
 * @file problem.cpp
 * @brief Problem class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#include <ostream>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "keywords.hpp"

using std::string;
using std::ostream;
using std::endl;

namespace mc_hybrid
{
  size_t
  Problem::get_variables_num() const
  {
    return vars.size();
  }

  Variable&
  Problem::get_variable(size_t idx)
  {
    return vars.at(idx);
  }

  size_t
  Problem::get_variables_num(Vars_group group) const
  {
    if (group < VARS_GROUPS_TOTAL)
      return vars_groups_idxs[group].size();
    else
      throw Vars_group_out_of_range();
  }

  Variable&
  Problem::get_variable(Vars_group group, size_t idx)
  {
    if (group < VARS_GROUPS_TOTAL)
      return get_variable(vars_groups_idxs[group].at(idx));
    else
      throw Vars_group_out_of_range();
  }

  size_t
  Problem::get_variable_idx(Vars_group group, size_t idx) const
  {
    if (group < VARS_GROUPS_TOTAL)
      return vars_groups_idxs[group].at(idx);
    else
      throw Vars_group_out_of_range();
  }

  void
  Problem::add_variable(Vars_group group, const Variable& v)
  {
    if (group < VARS_GROUPS_TOTAL)
    {
      vars.push_back(v);
      size_t idx = vars.size() - 1;
      vars_groups_idxs[group].push_back(idx);
      constrs_groups_vars_idxs[CONSTRS_TRANS].push_back(idx);
      if (group == VARS_STATE)
      {
        constrs_groups_vars_idxs[CONSTRS_INIT].push_back(idx);
        constrs_groups_vars_idxs[CONSTRS_SPEC].push_back(idx);
      }
    }
    else
      throw Vars_group_out_of_range();
  }

  size_t
  Problem::get_constraints_num() const
  {
    return constrs.size();
  }

  Constraint&
  Problem::get_constraint(size_t idx)
  {
    return constrs.at(idx);
  }

  size_t
  Problem::get_constraints_num(Constrs_group group) const
  {
    if (group < CONSTRS_GROUPS_TOTAL)
      return constrs_groups_idxs[group].size();
    else
      throw Constrs_group_out_of_range();
  }

  Constraint&
  Problem::get_constraint(Constrs_group group, size_t idx)
  {
    if (group < CONSTRS_GROUPS_TOTAL)
      return get_constraint(constrs_groups_idxs[group].at(idx));
    else
      throw Constrs_group_out_of_range();
  }

  size_t
  Problem::get_constraint_idx(Constrs_group group, size_t idx) const
  {
    if (group < CONSTRS_GROUPS_TOTAL)
      return constrs_groups_idxs[group].at(idx);
    else
      throw Constrs_group_out_of_range();
  }

  void
  Problem::add_constraint(Constrs_group group, const Constraint& c)
  {
    if (group < CONSTRS_GROUPS_TOTAL)
    {
      constrs.push_back(c);
      constrs_groups_idxs[group].push_back(constrs.size() - 1);
    }
    else
      throw Constrs_group_out_of_range();
  }

  size_t
  Problem::get_constraints_vars_num(Constrs_group group) const
  {
    if (group < CONSTRS_GROUPS_TOTAL)
      return constrs_groups_vars_idxs[group].size();
    else
      throw Constrs_group_out_of_range();
  }

  Variable&
  Problem::get_constraints_var(Constrs_group group, size_t idx)
  {
    if (group < CONSTRS_GROUPS_TOTAL)
      return get_variable(constrs_groups_vars_idxs[group].at(idx));
    else
      throw Constrs_group_out_of_range();
  }

  size_t
  Problem::get_constraints_var_idx(Constrs_group group, size_t idx) const
  {
    if (group < CONSTRS_GROUPS_TOTAL)
      return constrs_groups_vars_idxs[group].at(idx);
    else
      throw Constrs_group_out_of_range();
  }

  void
  Problem::clear()
  {
    vars.clear();
    for (size_t i = 0; i < VARS_GROUPS_TOTAL; ++i)
      vars_groups_idxs[i].clear();
    constrs.clear();
    for (size_t i = 0; i < CONSTRS_GROUPS_TOTAL; ++i)
    {
      constrs_groups_idxs[i].clear();
      constrs_groups_vars_idxs[i].clear();
    }
  }

  ostream&
  operator<<(ostream& s, Problem& p)
  {
    size_t i = 0;

    // writing variables
    s << keywords[KEYWORD_VARS_INPUT] << endl;
    for (i = 0; i < p.get_variables_num(Problem::VARS_INPUT); ++i)
    {
      s << "  " << p.get_variable(Problem::VARS_INPUT, i);
    }
    s << keywords[KEYWORD_VARS_STATE] << endl;
    for (i = 0; i < p.get_variables_num(Problem::VARS_STATE); ++i)
    {
      s << "  " << p.get_variable(Problem::VARS_STATE, i);
    }
    s << keywords[KEYWORD_VARS_OUTPUT] << endl;
    for (i = 0; i < p.get_variables_num(Problem::VARS_OUTPUT); ++i)
    {
      s << "  " << p.get_variable(Problem::VARS_OUTPUT, i);
    }
    // writing constraints
    s << keywords[KEYWORD_CONSTRS_INIT] << endl;
    for (i = 0; i < p.get_constraints_num(Problem::CONSTRS_INIT); ++i)
    {
      s << "  " << p.get_constraint(Problem::CONSTRS_INIT, i);
    }
    s << keywords[KEYWORD_CONSTRS_TRANS] << endl;
    for (i = 0; i < p.get_constraints_num(Problem::CONSTRS_TRANS); ++i)
    {
      s << "  " << p.get_constraint(Problem::CONSTRS_TRANS, i);
    }
    s << keywords[KEYWORD_CONSTRS_SPEC] << endl;
    for (i = 0; i < p.get_constraints_num(Problem::CONSTRS_SPEC); ++i)
    {
      s << "  " << p.get_constraint(Problem::CONSTRS_SPEC, i);
    }    return s;
  }
}; // namespace mc_hybrid
