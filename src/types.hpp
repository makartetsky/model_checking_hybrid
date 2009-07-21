/**
 * @file types.hpp
 * @brief Some typedefs and conversion functions declarations.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <sys/time.h>

#include <gmpxx.h>
#include <fm/rational.h>
#include <fm/system.h>
#include <fm/solution.h>
#include <fm/solver.h>

/**
 * @brief Project namespace.
 */
namespace mc_hybrid
{
  typedef mpq_class real_t;      ///< Real type (GMP rational).
  typedef mpz_class int_t;       ///< Integer type (GMP integer).
  typedef unsigned long ulong_t; ///< Abbreviation for unsigned long.

  /**
   * @brief Converts GMP rational into FM rational.
   *
   * @param[in] number GMP rational.
   *
   * @return FM rational.
   */
  s_fm_rational_t*
  real_t_to_fm_rational(real_t number);

  /**
   * @brief Converts FM rational into GMP rational.
   *
   * @param[in] number FM rational.
   *
   * @return GMP rational.
   */
  real_t
  fm_rational_to_real_t(s_fm_rational_t* number);

  /**
   * @brief Converts timeval struct into ms (milliseconds).
   *
   * @param[in] t Timeval struct object.
   *
   * @return Number of ms equivalent to t.
   */
  ulong_t
  timeval_to_ms(const timeval& t);
}; // namespace mc_hybrid

#endif // #ifndef TYPES_HPP_
