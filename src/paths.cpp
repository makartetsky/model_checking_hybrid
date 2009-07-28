/**
 * @file paths.cpp
 * @brief Paths constants definitions for NuSMV and Minisat+.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 26.07.2009
 */

namespace mc_hybrid
{
  const char* minisat_input_path = "./minisat_input";
  const char* minisat_output_path = "./minisat_output";
  const char* minisat_exec_path = "./minisat+.exe";

  const char* nusmv_input_path = "./nusmv_input";
  const char* nusmv_output_path = "./counterexample.xml";
  const char* nusmv_exec_path = "NuSMV";
  const char* nusmv_cmds_path = "./nusmv_cmds";
}; // namespace mc_hybrid
