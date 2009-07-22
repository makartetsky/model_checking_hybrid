/**
 * @file parser.hpp
 * @brief Parser class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <iosfwd>
#include <exception>

namespace mc_hybrid
{
  class Variable;
  class Constraint;
  class Problem;

  class Parser
  {
    public:
      /**
       * @brief Reads problem from the file.
       *
       * @param[in]  filename Path to the file.
       * @param[out] p        Problem.
       */
      void
      read(const std::string& filename, Problem& p);

    private:
      /**
       * @brief Reads variable from the file.
       *
       * @param[out] s Input stream.
       * @param[out] v Variable.
       */
      void
      read_variable(std::istream& s, Variable& v);

      /**
       * @brief Reads constraint from the file.
       *
       * @param[out] s Input stream.
       * @param[out] c Constrant.
       */
      void
      read_constraint(std::istream& s, Constraint& c);

      void
      generate_error_msg(const std::string& msg);

    private:
      std::string filename;
      size_t line_num;
      std::string error_msg;
  }; // class Parser
}; // namespacel mc_hybrid

#endif // #ifndef PARSER_HPP_
