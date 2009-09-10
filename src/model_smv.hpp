/**
 * @file model_smv.hpp
 * @brief Model_smv class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 19.07.2009
 */

#ifndef MODEL_SMV_HPP_
#define MODEL_SMV_HPP_

#include <vector>
#include <string>
#include <ostream>

namespace mc_hybrid
{
  class Problem;
  class Counterexample;

  /**
   * @brief SMV model representation.
   */
  class Model_smv
  {
    public:
      /**
       * @brief Constructor from Problem.
       *
       * @attention Problem must be pseudoboolean.
       *
       * @param[in] problem Problem.
       */
      Model_smv(Problem& problem);

      /**
       * @brief Refines model
       *
       * @return true if refinement succeeds, false otherwise.
       */
      bool
      refine(Counterexample& c);

      /**
       * @brief Writes model to file with specified name.
       *
       * @param[in] filename Filename.
       */
      void
      write(const std::string& filename);

      /**
       * @brief Verify model using NuSMV model checker.
       *
       * @param[in] ce Counterexample.
       *
       * @return true if model fits specification, false otherwise.
       */
      bool
      verify(Counterexample* ce);

      friend std::ostream&
      operator<<(std::ostream& s, Model_smv& m);

    public:
      std::vector<std::string> vars_input; ///< Input variables.
      std::vector<std::string> vars_state; ///< State variables.

      std::string init;  ///< Initial conditions.
      std::string trans; ///< Transitional relation.
      std::string spec;  ///< Specification.

    private:
      size_t tmax;                      ///< Maximal length of counterexample.
      std::vector<std::string> defines; ///< Defines.
  }; // class Model_smv

  /**
   * @brief Output operator for Problem class.
   *
   * @param[out] s Output stream.
   * @param[in]  m SMV model.
   *
   * @return Output stream.
   */
  std::ostream&
  operator<<(std::ostream& s, Model_smv& m);
}; // namespace mc_hybrid

#endif // #ifndef MODEL_SMV_HPP_
