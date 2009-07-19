/** 
 * @file variable.hpp
 * @brief Variable class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#ifndef VARIABLE_HPP_
#define VARIABLE_HPP_

#include <string>
#include <iostream>

namespace mc_hybrid
{
  /** 
   * @brief %Variable with name, type and range.
   */
  class Variable
  {
    public:
      /** 
       * @brief %Variable types enum.
       */
      enum Type
      {
        INTEGER = 0, ///< Integer type.
        REAL         ///< Real type.
      }; // enum Type

    public:
      /** 
       * @brief Default constructor.
       */
      Variable();

      /** 
       * @brief Constructor initializing all fields.
       * 
       * @param[in] name        %Variable name.
       * @param[in] type        %Variable type.
       * @param[in] lower_bound %Variable lower bound.
       * @param[in] upper_bound %Variable upper_bound.
       */
      Variable(const std::string& name, Type type,
               real_t lower_bound, real_t upper_bound);
      /** 
       * @brief Constructor from input stream.
       * 
       * @param[in] s Input stream.
       */
      Variable(std::istream& s);

      /** 
       * @brief Gets variable name.
       * 
       * @return %Variable name. 
       */
      const std::string&
      get_name() const;

      /** 
       * @brief Sets variable name.
       * 
       * @param[in] name New variable name.
       */
      void
      set_name(const std::string& name);

      /** 
       * @brief Gets variable type.
       * 
       * @return %Variable type.
       */
      Type
      get_type() const;

      /** 
       * @brief Sets variable type.
       * 
       * @param[in] type New variable type.
       */
      void
      set_type(Type type);

      /** 
       * @brief Gets variable lower bound.
       * 
       * @return %Variable lower bound.
       */
      real_t
      get_lower_bound() const;

      /** 
       * @brief Sets variable lower bound.
       * 
       * @param[in] lower_bound %Variable lower bound.
       */
      void
      set_lower_bound(real_t lower_bound);

      /** 
       * @brief Gets variable upper bound.
       * 
       * @return %Variable upper bound.
       */
      real_t
      get_upper_bound() const;

      /** 
       * @brief Sets variable upper bound.
       * 
       * @param[in] upper_bound %Variable upper bound.
       */
      void
      set_upper_bound(real_t upper_bound);

      friend std::ostream&
      operator<<(std::ostream& s, Variable& v);

      friend std::istream&
      operator>>(std::istream& s, Variable& v);

    private:
      std::string name;   ///< %Variable name.
      Type type;          ///< %Variable type.
      real_t lower_bound; ///< %Variable lower bound.
      real_t upper_bound; ///< %Variable upper bound.
  }; // class Variable

  /** 
   * @brief Output operator for Variable class.
   * 
   * @param[out] s Output stream.
   * @param[in]  v %Variable.
   * 
   * @return Output stream.
   */
  std::ostream&
  operator<<(std::ostream& s, Variable& v);

  /** 
   * @brief Input operator for Variable class.
   * 
   * @param[out] s Input stream.
   * @param[out] v %Variable.
   * 
   * @return Input stream.
   */
  std::istream&
  operator>>(std::istream& s, Variable& v);
}; // namespace mc_hybrid

#endif // #ifndef VARIABLE_HPP_
