/**
 * @file types.cpp
 * @brief Definition of functions from types.hpp.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 18.07.2009
 */

#include "types.hpp"

using std::vector;

namespace mc_hybrid
{
  s_fm_rational_t*
  real_t_to_fm_rational(real_t number)
  {
    s_fm_rational_t* result = fm_rational_alloc();
    mpz_set(result->num, number.get_num_mpz_t());
    mpz_set(result->denum, number.get_den_mpz_t());
    return result;
  }

  real_t
  fm_rational_to_real_t(s_fm_rational_t* number)
  {
    real_t result;
    mpz_set(result.get_num_mpz_t(), number->num);
    mpz_set(result.get_den_mpz_t(), number->denum);
    return result;
  }

  int_t
  lcf(const vector<int_t>& nums)
  {
    int_t result = 1;
    for (size_t i = 0; i < nums.size(); ++i)
      mpz_lcm(result.get_mpz_t(), result.get_mpz_t(), nums[i].get_mpz_t());
    return result;
  }

  ulong_t
  timeval_to_ms(const timeval& t)
  {
    return t.tv_sec * 1000 + t.tv_usec / 1000;
  }
}; // namespace mc_hybrid
