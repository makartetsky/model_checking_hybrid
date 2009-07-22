/**
 * @file fm_helpers.hpp
 * @brief Helper functions for using FM library declaration.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#ifndef FM_HELPERS_HPP_
#define FM_HELPERS_HPP_

namespace mc_hybrid
{
  /**
   * @brief Fills fm system from constraints group.
   *
   * @param[in] problem Problem.
   * @param[in] group   Constraints group.
   *
   * @return Filled fm system.
   */
  s_fm_system_t*
  fill_fm_system(Problem* problem,
                 Problem::Constrs_group group);
  /**
   * @brief Eliminates variables from constraints group.
   *
   * @param[in]  group               Constraints group.
   * @param[in]  vars_num            Number of variables to eliminate.
   * @param[in]  problem_source      Source problem
   * @param[out] problem_destination Destination problem.
   */
  void
  eliminate_variables(Problem::Constrs_group group,
                      size_t vars_num,
                      Problem* problem_source,
                      Problem* problem_destination);

}; // namespace mc_hybrid

#endif // #ifndef FM_HELPERS_HPP_
