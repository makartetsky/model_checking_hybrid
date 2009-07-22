/**
 * @file variable.cpp
 * @brief Variable class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#include <ostream>

#include "types.hpp"
#include "variable.hpp"

using std::string;
using std::ostream;
using std::endl;

namespace mc_hybrid
{
  Variable::Variable() : name(""), type(INTEGER),
                         lower_bound(0), upper_bound(0)
  {
  }

  Variable::Variable(const string& name, Type type,
                     real_t lower_bound, real_t upper_bound) :
                     name(name), type(type),
                     lower_bound(lower_bound), upper_bound(upper_bound)
  {
  }

  const std::string&
  Variable::get_name() const
  {
    return name;
  }

  void
  Variable::set_name(const std::string& name)
  {
    this->name = name;
  }

  Variable::Type
  Variable::get_type() const
  {
    return type;
  }

  void
  Variable::set_type(Type type)
  {
    this->type = type;
  }

  real_t
  Variable::get_lower_bound() const
  {
    return lower_bound;
  }

  void
  Variable::set_lower_bound(real_t lower_bound)
  {
    this->lower_bound = lower_bound;
  }

  real_t
  Variable::get_upper_bound() const
  {
    return upper_bound;
  }

  void
  Variable::set_upper_bound(real_t upper_bound)
  {
    this->upper_bound = upper_bound;
  }

  ostream&
  operator<<(ostream& s, Variable& v)
  {
    string str_type;
    if (v.type == Variable::INTEGER)
      str_type = "integer";
    else
      str_type = "real";
    return s << v.name << " " << str_type << " " << v.lower_bound << " .. " <<
           v.upper_bound << endl;
  }
}; // namespace mc_hybrid
