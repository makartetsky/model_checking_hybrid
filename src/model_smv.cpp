/**
 * @file model_smv.cpp
 * @brief Model_smv class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 25.07.2009
 */

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "types.hpp"
#include "model_smv.hpp"
#include "counterexample.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "minisat_helpers.hpp"
#include "paths.hpp"

using std::string;
using std::fstream;
using std::ios;
using std::runtime_error;
using std::endl;
using std::ostringstream;

namespace mc_hybrid
{
  Model_smv::Model_smv(Problem& problem)
  {
    // Fill variables names from problem.
    for (size_t i = 0; i < problem.get_variables_num(Problem::VARS_INPUT); ++i)
    {
      Variable& v = problem.get_variable(Problem::VARS_INPUT, i);
      vars_input.push_back(v.get_name());
    }
    for (size_t i = 0; i < problem.get_variables_num(Problem::VARS_STATE); ++i)
    {
      Variable& v = problem.get_variable(Problem::VARS_STATE, i);
      vars_state.push_back(v.get_name());
    }
    // Generate cnfs.
    size_t aux_num = 0;
    aux_num = minisat_launch(problem,
                             Problem::CONSTRS_INIT,
                             aux_num,
                             init);
    aux_num = minisat_launch(problem,
                             Problem::CONSTRS_TRANS,
                             aux_num,
                             trans);
    aux_num = minisat_launch(problem,
                             Problem::CONSTRS_SPEC,
                             aux_num,
                             spec);
    if (spec.length() == 0)
      throw std::logic_error("Specification is empty while creating SMV model.");

    for (size_t i = 0; i < aux_num; ++i)
    {
      ostringstream oss;
      oss << "_aux" << i;
      vars_state.push_back(oss.str());
    }
  }

  bool
  Model_smv::refine(Counterexample& /*counterexample*/)
  {
    return false;
  }

  void
  Model_smv::write(const string& filename)
  {
    fstream file(filename.c_str(), ios::out | ios::trunc);
    if (!file)
      throw runtime_error("Can't create NuSMV input file.");

    file << "MODULE main" << endl;
    file << "IVAR" << endl;
    for (size_t i = 0; i < vars_input.size(); ++i)
      file << "  " << vars_input.at(i) << " : 0 .. 1;" << endl;

    file << "VAR" << endl;
    for (size_t i = 0; i < vars_state.size(); ++i)
      file << "  " << vars_state.at(i) << " : 0 .. 1;" << endl;

    if (init.length() > 0)
    {
      file << "INIT" << endl;
      file << "  " << init << ";" << endl;
    }

    if (trans.length() > 0)
    {
      file << "TRANS" << endl;
      file << "  " << trans  << ";" << endl;
    }

    file << "SPEC" << endl;
    file << "  AG (" << spec << ");" << endl;
  }

  bool
  Model_smv::verify()
  {
    write(nusmv_input_path);

    //string nusmv_cmd = nusmv_exec_path;
    //nusmv_cmd += " ";
    //nusmv_cmd += nusmv_input_path;
    //nusmv_cmd += " > ";
    //nusmv_cmd += nusmv_output_path;

    //if (system(nusmv_cmd.c_str()) != 0)
      //throw runtime_error("NuSMV can't be launched.");
    
    //remove(nusmv_input_path);

    //// Read nusmv output.
    
    //remove(nusmv_output_path);

    return false;
  }
}; // namespace mc_hybrid
