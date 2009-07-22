/**
 * @file parser.cpp
 * @brief Parser class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#include <fstream>
#include <sstream>
#include <istream>
#include <stdexcept>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "parser.hpp"
#include "keywords.hpp"

using std::string;
using std::fstream;
using std::ios;
using std::istringstream;
using std::ostringstream;
using std::istream;
using std::runtime_error;
using std::logic_error;

namespace mc_hybrid
{
  void
  Parser::read(const string& filename, Problem& p)
  {
    this->filename = filename;
    fstream s(filename.c_str(), ios::in);
    if (!s)
      throw runtime_error("Input file " + filename + " doesn't exist.");

    p.clear();

    Variable v;
    Variable v_dash;
    Constraint c;
    istringstream is;
    int keyword_type = -1;
    string str;

    for (line_num = 1; getline(s, str); ++line_num)
    {
      if (str.length() == 0) // Empty line.
        continue;

      char symbol = str[0];
      if (symbol == '#') // Comment.
        continue;
      else if ((symbol != ' ') && (symbol != '\t')) // Keyword.
      {
        int i = 0;
        for (; i <  KEYWORDS_TOTAL; ++i)
        {
          if (str == keywords[i])
            break;
        }
        if (i == KEYWORDS_TOTAL)
        {
          generate_error_msg("Unknown keyword \"" + str + "\".");
          throw logic_error(error_msg.c_str());
        }
        if (i == keyword_type + 1)
          keyword_type = i;
        else
        {
          generate_error_msg("Unexpected keyword \"" + str + "\".");
          throw logic_error(error_msg.c_str());
        }
      }
      else // Variable or constraint.
      {
        is.str(str);
        switch (keyword_type)
        {
          case KEYWORD_VARS_INPUT:
            read_variable(is, v);
            p.add_variable(Problem::VARS_INPUT, v);
            break;
          case KEYWORD_VARS_STATE:
            read_variable(is, v);
            p.add_variable(Problem::VARS_STATE, v);
            v_dash = v;
            v_dash.set_name(v.get_name() + "'");
            p.add_variable(Problem::VARS_NEXT_STATE, v_dash);
            break;
          case KEYWORD_VARS_OUTPUT:
            read_variable(is, v);
            p.add_variable(Problem::VARS_OUTPUT, v);
            break;
          case KEYWORD_CONSTRS_INIT:
            read_constraint(is, c);
            p.add_constraint(Problem::CONSTRS_INIT, c);
            break;
          case KEYWORD_CONSTRS_TRANS:
            read_constraint(is, c);
            p.add_constraint(Problem::CONSTRS_TRANS, c);
            break;
          case KEYWORD_CONSTRS_SPEC:
            read_constraint(is, c);
            p.add_constraint(Problem::CONSTRS_SPEC, c);
            break;
        }
      }
    }
  }

  void
  Parser::read_variable(istream& s, Variable& v)
  {
    string str;

    s >> v.name >> str;
    if (str == "integer")
      v.type = Variable::INTEGER;
    else if (str == "real")
      v.type = Variable::REAL;
    else
    {
      generate_error_msg("Unknown variable type \"" + str + "\".");
      throw logic_error(error_msg.c_str());
    }

    s >> v.lower_bound;
    v.lower_bound.canonicalize();
    s >> str;
    if (str != "..")
    {
      generate_error_msg("Unexpected string \"" + str + "\".");
      throw logic_error(error_msg.c_str());
    }
    s >> v.upper_bound;
    v.upper_bound.canonicalize();

    if (v.upper_bound < v.lower_bound)
    {
      generate_error_msg("Incorrect variable range.");
      throw logic_error(error_msg.c_str());
    }
  }

  void
  Parser::read_constraint(istream& s, Constraint& c)
  {
    real_t coeff;
    real_t free_member;
    string str;
    int sign = 1;
    char symbol;

    c.coeffs.clear();

    for (;;)
    {
      s >> coeff;
      coeff.canonicalize();
      s >> str;
      if (c.coeffs.insert(make_pair(str, coeff)).second == 0)
        c.coeffs[str] += coeff;

      s.ignore();
      symbol = s.peek();
      if ((symbol == '>') || (symbol == '='))
        break;
      else if (symbol == '<')
      {
        sign = -1;
        break;
      }
      else if (symbol == '+')
        s.ignore();
    }

    s >> str;
    if (str == "=")
      c.type = Constraint::EQUAL;
    else if (str == ">=" || str == "<=")
      c.type = Constraint::MORE_OR_EQUAL;
    else
    {
      generate_error_msg("Unknown constraint type \"" + str + "\".");
      throw logic_error(error_msg.c_str());
    }

    s >> free_member;
    free_member.canonicalize();
    c.free_member = -1 * free_member;

    if (sign < 0)
      c.mult(sign);
  }

  void
  Parser::generate_error_msg(const string& msg)
  {
    ostringstream s;
    s << filename << "|" << line_num << "| error: " << msg;
    error_msg = s.str();
  }
}; // namespace mc_hybrid
