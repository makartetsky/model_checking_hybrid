/** 
 * @file problem.hpp
 * @brief Problem class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#ifndef PROBLEM_HPP_
#define PROBLEM_HPP_

#include <vector>
#include <stdexcept>

namespace mc_hybrid
{
  /** 
   * @brief Input data representation.
   */
  class Problem
  {
    public:
      /** 
       * @brief Variables groups enum.
       */
      enum Vars_group
      {
        VARS_INPUT = 0,   ///< Input variables.
        VARS_STATE,       ///< State variables.
        VARS_NEXT_STATE,  ///< Next state variables.
        VARS_OUTPUT,      ///< Output variables.
        VARS_GROUPS_TOTAL ///< Total count of variables groups.
      }; // enum Vars_group

      class Vars_group_out_of_range : public std::out_of_range
      {
        public:
          Vars_group_out_of_range() :
            std::out_of_range("Variables group is out of range.")
          {
          }
      }; // class Vars_group_out_of_range

      /** 
       * @brief Constrs groups enum.
       */
      enum Constrs_group
      {
        CONSTRS_INIT,        ///< Initial conditions constraints.
        CONSTRS_TRANS,       ///< Transitional relation constraints.
        CONSTRS_SPEC,        ///< Specification constraints.
        CONSTRS_GROUPS_TOTAL ///< Total count of constraints groups.
      }; // enum Constrs_group

      class Constrs_group_out_of_range : public std::out_of_range
      {
        public:
          Constrs_group_out_of_range() :
            std::out_of_range("Constrs group is out of range.")
          {
          }
      }; // class Constrs_group_out_of_range

    public:
      /** 
       * @brief Default constructor.
       */
      Problem();

      /** 
       * @brief Constructor from input stream.
       * 
       * @param[in] s Input stream.
       */
      Problem(std::istream& s);

      /** 
       * @brief Gets variables number.
       * 
       * @return Variables number.
       */
      size_t
      get_variables_num() const;

      /** 
       * @brief Gets variable with specified index.
       * 
       * @param[in] idx Variable index.
       * 
       * @return Variable.
       */
      Variable&
      get_variable(size_t idx);

      /** 
       * @brief Gets variables num for specified variables group.
       * 
       * @param[in] group Variables group.
       * 
       * @return Variables number.
       */
      size_t
      get_variables_num(Vars_group group) const;

      /** 
       * @brief Gets variable with specified index.
       * 
       * @param[in] group Variables group.
       * @param[in] idx   Variable index.
       * 
       * @return Variable.
       */
      Variable&
      get_variable(Vars_group group, size_t idx);

      /** 
       * @brief Gets global variable index by index in the variables group.
       * 
       * @param[in] group Variables group.
       * @param[in] idx   Variable index in the variables group.
       * 
       * @return Global variable index.
       */
      size_t
      get_variable_idx(Vars_group group, size_t idx) const;

      /** 
       * @brief Adds variable to specified group.
       * 
       * @param[in] group Variables group.
       * @param[in] v     Variable.
       */
      void
      add_variable(Vars_group group, const Variable& v);

      /** 
       * @brief Gets constraints number.
       * 
       * @return Constraints number.
       */
      size_t
      get_constraints_num() const;

      /** 
       * @brief Gets constraint with specified index.
       * 
       * @param[in] idx Constraint index.
       * 
       * @return Constraint.
       */
      Constraint&
      get_constraint(size_t idx);

      /** 
       * @brief Gets constraints number for specified group.
       * 
       * @param[in] group Constraints group.
       * 
       * @return Constraints number.
       */
      size_t
      get_constraints_num(Constrs_group group) const;

      /** 
       * @brief Gets constraint with specified index
       * @brief for specified constraints group.
       * 
       * @param[in] group Constrs group.
       * @param[in] idx   Constraint index.
       * 
       * @return Constraint.
       */
      Constraint&
      get_constraint(Constrs_group group, size_t idx);

      /** 
       * @brief Gets global constraint index by index in the constraints group.
       * 
       * @param[in] group Constraints group.
       * @param[in] idx   Constraint index in the constraints group.
       * 
       * @return Global constraint index.
       */
      size_t
      get_constraint_idx(Constrs_group group, size_t idx) const;

      /** 
       * @brief Adds constraint to specified group.
       * 
       * @param[in] group Constraints group.
       * @param[in] c     Constraint.
       */
      void
      add_constraint(Constrs_group group, const Constraint& c);

      /** 
       * @brief Get variables number for specified constraints group.
       * 
       * @param[in] group Constraints group.
       * 
       * @return Variables number.
       */
      size_t
      get_constraints_vars_num(Constrs_group group) const;

      /** 
       * @brief Gets variable with specified index
       * @brief for specified constraints group.
       * 
       * @param[in] group Constraints group.
       * @param[in] idx   Variable index.
       * 
       * @return Variable.
       */
      Variable&
      get_constraints_var(Constrs_group group, size_t idx);

      /** 
       * @brief Gets variable index by index in the constraints group.
       * 
       * @param[in] group Constraints group.
       * @param[in] idx   Variable index in the constraints group.
       * 
       * @return Global variable index.
       */
      size_t
      get_constraints_var_idx(Constrs_group group, size_t idx) const;

      friend std::ostream&
      operator<<(std::ostream& s, Problem& p);

      friend std::istream&
      operator>>(std::istream& s, Problem& p);

    private:
      /** 
       * @brief Clear arrays.
       */
      void
      clear();

    private:
      /** 
       * @brief Variables array.
       */
      std::vector<Variable> vars;
      /** 
       * @brief Array of variables idxs for each variables group.
       */
      std::vector<size_t> vars_groups_idxs[VARS_GROUPS_TOTAL];
      /** 
       * @brief Constrs array.
       */
      std::vector<Constraint> constrs;
      /** 
       * @brief Array of constraints idxs for each constraints group.
       */
      std::vector<size_t> constrs_groups_idxs[CONSTRS_GROUPS_TOTAL];
      /** 
       * @brief Array of variables idxs for each constraints group.
       */
      std::vector<size_t> constrs_groups_vars_idxs[CONSTRS_GROUPS_TOTAL];
  }; // class Problem

  /** 
   * @brief Output operator for Problem class.
   * 
   * @param[out] s Output stream.
   * @param[in]  p Problem.
   * 
   * @return Output stream.
   */
  std::ostream&
  operator<<(std::ostream& s, Problem& p);

  /** 
   * @brief Input operator for Problem class.
   * 
   * @param[out] s Input stream.
   * @param[out] p Problem.
   * 
   * @return Input stream.
   */
  std::istream&
  operator>>(std::istream& s, Problem& p);
}; // namespace mc_hybrid

#endif // #ifndef PROBLEM_HPP_
