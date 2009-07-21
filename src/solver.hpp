/**
 * @file solver.hpp
 * @brief Solver class definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#ifndef SOLVER_HPP_
#define SOLVER_HPP_

#include <vector>
#include <map>
#include <string>

namespace mc_hybrid
{
  class Problem;
  class Model_smv;
  class Counterexample;

  /**
   * @brief Application logic.
   *
   * @see Problem
   * @see Model_smv
   * @see Counterexample
   */
  class Solver
  {
    public:
      /**
       * @brief Default constructor.
       *
       * This constructor assigns initial values to class fields.
       */
      Solver();

      /**
       * @brief Destructor.
       *
       * Frees memory allocated for class fields.
       */
      ~Solver();

      /**
       * @brief Verifies hybrid system.
       *
       * Verifies given hybrid system with given initial quantization
       * parameter value.
       *
       * @param[in] filename        Path to the input file.
       * @param[in] initial_q_param Initial value of quantization param.
       * @param[in] verbosity       Verbosity value.
       *
       * @return Duration of verification process.
       */
      ulong_t
      verify(const std::string& filename, int initial_q_param, int verbosity);

      /**
       * @brief Gets counterexample.
       *
       * Gets pointer to counterexample. Pointer is valid until Solver
       * object is live.
       *
       * @attention Must be called after verify method.
       *
       * @return Constant pointer to counterexample if verification process
       * @return has failed, 0 otherwise
       */
      const Counterexample*
      get_counterexample() const;

    private:
      /**
       * @brief Makes problem without output variables.
       *
       * Make problem "equivalent" to original problem, but without output
       * variables.
       */
      void
      make_problem_without_outputs();

      /**
       * @brief Makes quantized problem.
       *
       * Makes problem "equivalent" to problem without output variables by
       * applying quantization process.
       */
      void
      make_problem_quantized();

      /**
       * @brief Makes discrete problem.
       *
       * Makes problem "equivalent" to quantized problem, but containing
       * only discrete variables.
       */
      void
      make_problem_discrete();

      /**
       * @brief Makes pseudoboolean problem.
       *
       * Makes problem "equivalent" to discrete problem, but containing
       * only pseudoboolean variables.
       */
      void
      make_problem_pb();

      /**
       * @brief Makes SMV model.
       *
       * Makes SMV model equivalent to pseudoboolean problem.
       */
      void
      make_model_smv();

      /**
       * @brief Verifies SMV model.
       *
       * Verifies SMV model using NuSMV model checker.
       *
       * @return true if model fits specification, false otherwise.
       */
      bool
      verify_model_smv();

      /**
       * @brief Checks if counterexample is spurious.
       *
       * Checks if counterexample found by NuSMV is spurious in case of
       * original problem using GLPK.
       *
       * @return true if counterexample is true, false otherwise.
       */
      bool
      check_counterexample();

      /**
       * @brief Refines SMV model.
       *
       * Tries to refine SMV model by "allowing" counterexample.
       *
       * @return true if refinement succeeds, false otherwise.
       */
      bool
      refine_model_smv();

      /**
       * @brief Changes quantization parameters values.
       *
       * Changes values of quantization parameter for real variables.
       */
      void
      change_q_params();

    private:
      /**
       * @brief Quantization parameters for real variables.
       */
      std::map<std::string, size_t> q_params;

      /**
       * @brief Original problem.
       */
      Problem* problem_original;
      /**
       * @brief Problem without output variables.
       */
      Problem* problem_without_outputs;
      /**
       * @brief Quantized problem.
       */
      Problem* problem_quantized;
      /**
       * @brief Discrete problem.
       */
      Problem* problem_discrete;
      /**
       * @brief Pseudoboolean problem.
       */
      Problem* problem_pb;

      /**
       * @brief Mapping between integer and boolean variables.
       */
      std::vector<std::vector<size_t> > vars_mapping;

      /**
       * @brief SMV model.
       */
      Model_smv* model_smv;

      /**
       * @brief Counterexample for verification of boolean model.
       */
      Counterexample* counterexample_boolean;
      /**
       * @brief Counterexample for verification of discrete model.
       */
      Counterexample* counterexample_discrete;
      /**
       * @brief Counterexample for verification of original model.
       */
      Counterexample* counterexample_mixed;
  }; // class Solver
}; // namespace mc_hybrid

#endif // #ifndef SOLVER_HPP_
