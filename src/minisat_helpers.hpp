/**
 * @file minisat_helpers.hpp
 * @brief Helper functions for using minisat+ declaration.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#ifndef MINISAT_HELPERS_HPP_
#define MINISAT_HELPERS_HPP_

#include <string>

namespace mc_hybrid
{
  /**
   * @brief Makes cnf equisatisfiable to system of pb constraints.
   *
   * @param[in]  problem Problem.
   * @param[in]  group   Constraints group.
   * @param[in]  aux_num Aux vars number.
   * @param[out] cnf     Result string with CNF.
   *
   * @return New aux vars number.
   */
  size_t
  minisat_launch(Problem* problem,
          Problem::Constrs_group group,
          size_t aux_num,
          std::string& cnf);

  /**
   * @brief Makes input file for minisat+.
   *
   * @param[in]  problem Problem.
   * @param[in]  group   Constraints group.
   */
  void
  minisat_generate_input(Problem* problem,
                         Problem::Constrs_group group);

  /**
   * @brief Reads minisat output file.
   *
   * @param[in]  aux_num Aux vars number.
   * @param[out] cnf     Result string with CNF.
   *
   * @return New aux vars number.
   */
  size_t
  minisat_read_output(size_t aux_num,
                      std::string& cnf);
}; // namespace mc_hybrid

#endif // #ifndef MINISAT_HELPERS_HPP_
