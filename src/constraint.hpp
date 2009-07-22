/**
 * @file constraint.hpp
 * @brief Constraint class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#ifndef CONSTRAINT_HPP_
#define CONSTRAINT_HPP_

#include <map>
#include <iosfwd>

namespace mc_hybrid
{
  class Parser;

  /**
   * @brief Algebraic constraint.
   */
  class Constraint
  {
    public:
      /**
       * @brief Constraint types enum.
       */
      enum Type
      {
        EQUAL = 0,    ///< Equality constraint.
        MORE_OR_EQUAL ///< Inequality constraint.
      }; // enum Type

    public:
      /**
       * @brief Default constructor.
       */
      Constraint();

      /**
       * @brief Constructor specifying type.
       *
       * @param[in] type Constraint type.
       */
      Constraint(Type type);

      /**
       * @brief Gets constraint type.
       *
       * @return %Constraint type.
       */
      Type
      get_type() const;

      /**
       * @brief Sets constraint type
       *
       * @param[in] type New constraint type.
       */
      void
      set_type(Type type);

      /**
       * @brief Gets coefficient for specified variable name.
       *
       * @param[in] var_name Variable name.
       *
       * @return Coefficient.
       */
      real_t
      get_coeff(const std::string& var_name) const;

      /**
       * @brief Sets coefficient for specified variable name.
       *
       * @param[in] var_name Variable name.
       * @param[in] coeff    New coefficient.
       */
      void
      set_coeff(const std::string& var_name, real_t coeff);

      /**
       * @brief Gets coefficient for specified variable.
       *
       * @param[in] v Variable.
       *
       * @return Coefficient.
       */
      real_t
      get_coeff(const Variable& v) const;

      /**
       * @brief Sets coefficient for specified variable.
       *
       * @param[in] v     Variable.
       * @param[in] coeff New coefficient.
       */
      void
      set_coeff(const Variable& v, real_t coeff);

      /**
       * @brief Gets free member.
       *
       * @return Free member.
       */
      real_t
      get_free_member() const;

      /**
       * @brief Sets free member.
       *
       * @param[in] free_member Free member.
       */
      void
      set_free_member(real_t free_member);

      /**
       * @brief Multiplies coefficients and free_member by given number.
       *
       * @param num Number.
       */
      void
      mult(real_t num);

      friend std::ostream&
      operator<<(std::ostream& s, Constraint& c);

      friend class Parser;

    private:
      Type type;                            ///< %Constraint type.
      std::map<std::string, real_t> coeffs; ///< %Constraint coefficients.
      real_t free_member;                   ///< %Constraint free member.
  }; // class Constraint

  /**
   * @brief Output operator for Constraint class.
   *
   * @param[out] s Output stream
   * @param[in]  c %Constraint.
   *
   * @return Output stream.
   */
  std::ostream&
  operator<<(std::ostream& s, Constraint& c);
}; // namespace mc_hybrid

#endif // #ifndef CONSTRAINT_HPP_
