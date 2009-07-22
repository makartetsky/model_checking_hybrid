/**
 * @file keywords.hpp
 * @brief Keywords for Problem reading/writing declaration.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#ifndef KEYWORDS_HPP_
#define KEYWORDS_HPP_

namespace mc_hybrid
{
  /**
   * @brief Keywords enum.
   */
  enum Keywords
  {
    KEYWORD_VARS_INPUT,
    KEYWORD_VARS_STATE,
    KEYWORD_VARS_OUTPUT,
    KEYWORD_CONSTRS_INIT,
    KEYWORD_CONSTRS_TRANS,
    KEYWORD_CONSTRS_SPEC,
    KEYWORDS_TOTAL
  }; // enum Keywords

  /**
   * @brief Keywords strings.
   */
  extern const char* keywords[KEYWORDS_TOTAL];
}; // namespace mc_hybrid

#endif // #ifndef KEYWORDS_HPP_
