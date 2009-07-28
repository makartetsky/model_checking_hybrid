/**
 * @file minisat_helpers.cpp
 * @brief Helper functions for minisat+ using definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 22.07.2009
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "minisat_helpers.hpp"
#include "paths.hpp"

using std::string;
using std::ostringstream;
using std::fstream;
using std::endl;
using std::ios;
using std::remove;
using std::runtime_error;
using std::vector;
using std::istringstream;

namespace mc_hybrid
{
  size_t
  minisat_launch(Problem& problem,
                 Problem::Constrs_group group,
                 size_t aux_num,
                 string& cnf)
  {
    minisat_generate_input(problem, group);

    string minisat_cmd(minisat_exec_path);
    minisat_cmd += " ";
    minisat_cmd += minisat_input_path;
    minisat_cmd += " -cnf=";
    minisat_cmd += minisat_output_path;
    minisat_cmd += " > /dev/null";
    int result = system(minisat_cmd.c_str());
    if (result != 0)
      throw runtime_error("Minisat+ can't be launched");
    remove(minisat_input_path);

    size_t new_aux_num = minisat_read_output(aux_num, cnf);

    remove(minisat_output_path);

    return new_aux_num;
  }

  void
  minisat_generate_input(Problem& problem,
                         Problem::Constrs_group group)
  {
    fstream file(minisat_input_path, ios::out | ios::trunc);
    if (!file)
      throw runtime_error("Can't create minisat+ input file.");

    for (size_t i = 0; i < problem.get_constraints_num(group); ++i)
    {
      Constraint& c = problem.get_constraint(group, i);
      for (size_t j = 0; j < problem.get_constraints_vars_num(group); ++j)
      {
        Variable& v = problem.get_constraints_var(group, j);
        real_t coeff = c.get_coeff(v);
        if (coeff != 0)
          file << coeff << "*" << v.get_name() << " ";
      }
      file << ">= " << -1 * c.get_free_member() << ";" << endl;
    }
  }

  size_t
  minisat_read_output(size_t aux_num,
                      string& cnf)
  {
    fstream file(minisat_output_path, ios::in);
    if (!file)
      throw runtime_error("Minisat+ output file doesn't exist.");

    // Read vars and clauses num.
    size_t vars_num;
    file >> vars_num;
    size_t clauses_num;
    file >> clauses_num;
    if (clauses_num > 0)
    {
      file.ignore();
      // read names
      vector<string> index2name;
      string names;
      getline(file, names);
      istringstream iss(names);
      string name;
      while (iss >> name)
      {
        // replace ' with next()
        if (name[name.length() - 1] == '\'')
        {
          name.erase(name.length() - 1);
          name = "next(" + name + ")";
        }
        index2name.push_back(name);
      }
      size_t orig_number = index2name.size();

      // Add enough num of aux vars to index2name.
      size_t aux_index = aux_num;
      for (size_t i = orig_number; i < vars_num; ++i)
      {
        ostringstream oss;
        oss << "_aux" << aux_index;
        index2name.push_back(oss.str());
        ++aux_index;
      }

      // Read clauses.
      ostringstream oss_result;
      string clause;
      while (getline(file, clause))
      {
        istringstream iss(clause);
        oss_result << "(";
        int var_index;
        while (iss >> var_index)
        {
          if (var_index < 0)
          {
            oss_result << "!";
            var_index *= -1;
          }
          oss_result << index2name.at(var_index);
          oss_result << " | ";
        }
        oss_result.seekp(-3, ios::end);
        oss_result << ") & ";
      }
      cnf = oss_result.str();
      cnf.erase(cnf.length() - 3);
      return aux_index;
    }
    else
    {
      cnf = "";
      return 0;
    }
  }
}; // namespace mc_hybrid
