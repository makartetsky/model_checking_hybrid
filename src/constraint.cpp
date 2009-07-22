/**
 * @file constraint.cpp
 * @brief Constraint class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#include <ostream>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"

using std::string;
using std::ostream;
using std::endl;
using std::map;

namespace mc_hybrid
{
  Constraint::Constraint() : type(EQUAL), free_member(0)
  {
  }

  Constraint::Constraint(Type type) : type(type), free_member(0)
  {
  }

  Constraint::Type
  Constraint::get_type() const
  {
    return type;
  }

  void
  Constraint::set_type(Type type)
  {
    this->type = type;
  }

  real_t
  Constraint::get_coeff(const string& var_name) const
  {
    if (coeffs.count(var_name) != 0)
      return coeffs.find(var_name)->second;
    else
      return 0;
  }

  void
  Constraint::set_coeff(const string& var_name, real_t coeff)
  {
    if (coeff != 0)
      coeffs[var_name] = coeff;
    else
      coeffs.erase(var_name);
  }

  real_t
  Constraint::get_coeff(const Variable& v) const
  {
    return get_coeff(v.get_name());
  }

  void
  Constraint::set_coeff(const Variable& v, real_t coeff)
  {
    set_coeff(v.get_name(), coeff);
  }

  real_t
  Constraint::get_free_member() const
  {
    return free_member;
  }

  void
  Constraint::set_free_member(real_t free_member)
  {
    this->free_member = free_member;
  }

  void
  Constraint::mult(real_t num)
  {
    map<string, real_t>::iterator iter;
    for (iter = coeffs.begin(); iter != coeffs.end(); ++iter)
      iter->second *= num;
    free_member *= num;
  }

  ostream&
  operator<<(ostream& s, Constraint& c)
  {
    map<string, real_t>::iterator iter;
    for (iter = c.coeffs.begin (); iter != c.coeffs.end(); )
    {
      s << iter->second << " " << iter->first << " ";
      if (++iter != c.coeffs.end())
      {
        s << "+ ";
      }
    }
    if (c.type == Constraint::EQUAL)
      s << "= ";
    else
      s << ">= ";
    s << -1 * c.free_member;

    return s << endl;
  }
}; // namespace mc_hybrid
