/**
 * @file counterexample.hpp
 * @brief Counterexample class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 19.07.2009
 */

#ifndef COUNTEREXAMPLE_HPP_
#define COUNTEREXAMPLE_HPP_

#include <vector>
#include <map>
#include <string>

namespace mc_hybrid
{
  class Problem;
  class Model_smv;

  /**
   * @brief %Counterexample for verification process.
   */
  class Counterexample
  {
    public:
      /**
       * @brief This constructor extracts variables names from problem.
       *
       * @param[in] problem %Problem.
       */
      Counterexample(Problem& problem);

      /**
       * @brief This constructor extracts variables names from model_smv.
       *
       * @param[in] model_smv SMV model.
       */
      Counterexample(Model_smv& model_smv);

      /**
       * @brief Read variables values from XML file.
       *
       * @param[in] filename Path to the XML file.
       */
      void
      read(const std::string& filename);

      /**
       * @brief Gets steps count.
       *
       * @return Steps count.
       */
      size_t
      get_steps_count() const;

      /**
       * @brief Add step. Variables values are copied from previous step.
       */
      void
      add_step();

      /**
       * @brief Gets input variables count.
       *
       * @return Input variables count.
       */
      size_t
      get_ivars_count() const;

      /**
       * @brief Gets variable value.
       *
       * @param[in] step     Step number.
       * @param[in] var_name %Variable name.
       *
       * @return %Variable value.
       */
      real_t
      get_ivar_value(size_t step,
                     const std::string& var_name) const;

      /**
       * @brief Sets value for input variable.
       *
       * @param[in] step     Step number.
       * @param[in] var_name %Variable name.
       * @param[in] value    %Variable value.
       */
      void
      set_ivar_value(size_t step,
                     const std::string& var_name,
                     real_t value);

      /**
       * @brief Gets state variables count.
       *
       * @return State variables count.
       */
      size_t
      get_vars_count() const;

      /**
       * @brief Gets variable value.
       *
       * @param[in] step     Step number.
       * @param[in] var_name %Variable name.
       *
       * @return %Variable value.
       */
      real_t
      get_var_value(size_t step,
                    const std::string& var_name) const;

      /**
       * @brief Sets value for state variable.
       *
       * @param[in] step     Step number.
       * @param[in] var_name %Variable name.
       * @param[in] value    %Variable value.
       */
      void
      set_var_value(size_t step,
                    const std::string& var_name,
                    real_t value);

    private:
      /**
       * @brief Steps number.
       */
      size_t steps_count;
      /**
       * @brief Input variables names.
       */
      std::map<std::string, size_t> vars_input_names;
      /**
       * @brief State variables names.
       */
      std::map<std::string, size_t> vars_state_names;
      /**
       * @brief Input variables values.
       */
      std::vector<std::vector<real_t> > vars_input_values;
      /**
       * @brief State variables values.
       */
      std::vector<std::vector<real_t> > vars_state_values;
  }; // class Counterexample
}; // namespace mc_hybrid

#endif // #ifndef COUNTEREXAMPLE_HPP_
