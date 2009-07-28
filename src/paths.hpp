/**
 * @file paths.hpp
 * @brief Paths constants declarations for NuSMV and Minisat+.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 26.07.2009
 */

#ifndef PATHS_HPP_
#define PATHS_HPP_

namespace mc_hybrid
{
  extern const char* minisat_input_path;  ///< Path to Minisat+ input file.
  extern const char* minisat_output_path; ///< Path to Minisat+ output file.
  extern const char* minisat_exec_path;   ///< Path to Minisat+ executable.

  extern const char* nusmv_input_path;  ///< Path to NuSMV input file.
  extern const char* nusmv_output_path; ///< Path to NuSMV output file.
  extern const char* nusmv_exec_path;   ///< Path to NuSMV executable.
  extern const char* nusmv_cmds_path;   ///< Path to the file with commands for NuSMV interactive mode.
}; // namespace mc_hybrid

#endif // #ifndef PATHS_HPP_
