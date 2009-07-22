/**
 * @file keywords.cpp
 * @brief Keywords for Problem reading/writing definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#include "keywords.hpp"

namespace mc_hybrid
{
  const char* keywords[KEYWORDS_TOTAL] =
  {
    "VARS_INPUT",
    "VARS_STATE",
    "VARS_OUTPUT",
    "CONSTRS_INIT",
    "CONSTRS_TRANS",
    "CONSTRS_SPEC"
  };
}; // namespace mc_hybrid
