/**
 * @file counterexample.cpp
 * @brief Counterexample class methods definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 27.07.2009
 */

#include  <stdexcept>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "types.hpp"
#include "variable.hpp"
#include "constraint.hpp"
#include "problem.hpp"
#include "model_smv.hpp"
#include "counterexample.hpp"

using std::string;
using std::logic_error;

using xercesc::XercesDOMParser;
using xercesc::DOMDocument;
using xercesc::DOMElement;
using xercesc::DOMNode;
using xercesc::DOMNodeList;
using xercesc::XMLString;

namespace mc_hybrid
{
  Counterexample::Counterexample(Problem& problem)
  {
    for (size_t i = 0; i < problem.get_variables_num(Problem::VARS_INPUT); ++i)
    {
      Variable& v = problem.get_variable(Problem::VARS_INPUT, i);
      vars_input_names.insert(make_pair(v.get_name(), i));
    }

    for (size_t i = 0; i < problem.get_variables_num(Problem::VARS_STATE); ++i)
    {
      Variable& v = problem.get_variable(Problem::VARS_STATE, i);
      vars_state_names.insert(make_pair(v.get_name(), i));
    }

    steps_count = 0;
  }

  Counterexample::Counterexample(Model_smv& model_smv)
  {
    for (size_t i = 0; i < model_smv.vars_input.size(); ++i)
    {
      string& name = model_smv.vars_input.at(i);
      vars_input_names.insert(make_pair(name, i));
    }

    for (size_t i = 0; i < model_smv.vars_state.size(); ++i)
    {
      string& name = model_smv.vars_state.at(i);
      vars_state_names.insert(make_pair(name, i));
    }

    steps_count = 0;
  }

  void
  Counterexample::read(const string& filename)
  {
    XMLCh* str_node = XMLString::transcode("node");
    XMLCh* str_state = XMLString::transcode("state");
    XMLCh* str_input = XMLString::transcode("input");
    XMLCh* str_value = XMLString::transcode("value");
    XMLCh* str_variable = XMLString::transcode("variable");

    XercesDOMParser* parser = new XercesDOMParser();
    parser->parse(filename.c_str());
    DOMDocument* document = parser->getDocument();
    DOMNodeList* steps_nodes =
      document->getElementsByTagName(str_node);
    for (size_t i = 0; i < steps_nodes->getLength(); ++i)
    {
      add_step();

      DOMElement* step_node =
        (DOMElement*) steps_nodes->item(i);

      DOMElement* state_node =
        (DOMElement*) step_node->getElementsByTagName(str_state)->item(0);
      DOMNodeList* state_values_nodes =
        state_node->getElementsByTagName(str_value);
      for (size_t j = 0; j < state_values_nodes->getLength(); ++j)
      {
        DOMElement* state_value_node = 
          (DOMElement*) state_values_nodes->item(j);
        char* var_name = XMLString::transcode(
            state_value_node->getAttribute(str_variable));
        char* var_value = XMLString::transcode(
            state_value_node->getNodeValue());
        set_var_value(i, var_name, real_t(var_value));
        XMLString::release(&var_name);
        XMLString::release(&var_value);
      }

      DOMElement* input_node =
        (DOMElement*) step_node->getElementsByTagName(str_input)->item(0);
      DOMNodeList* input_values_nodes =
        input_node->getElementsByTagName(str_value);
      for (size_t j = 0; j < input_values_nodes->getLength(); ++j)
      {
        DOMElement* input_value_node = 
          (DOMElement*) input_values_nodes->item(j);
        char* var_name = XMLString::transcode(
            input_value_node->getAttribute(str_variable));
        char* var_value = XMLString::transcode(
            input_value_node->getNodeValue());
        set_ivar_value(i, var_name, real_t(var_value));
        XMLString::release(&var_name);
        XMLString::release(&var_value);
      }
    }
    delete parser;

    XMLString::release(&str_node);
    XMLString::release(&str_state);
    XMLString::release(&str_input);
    XMLString::release(&str_value);
    XMLString::release(&str_variable);
  }

  size_t
  Counterexample::get_steps_count() const
  {
    return steps_count;
  }

  void
  Counterexample::add_step()
  {
    if (steps_count > 0)
    {
      vars_input_values.push_back(vars_input_values.back());
      vars_state_values.push_back(vars_state_values.back());
    }
    else
    {
      vars_state_values.resize(1);
      vars_state_values.at(0).resize(vars_state_names.size());
      vars_input_values.resize(1);
      vars_input_values.at(0).resize(vars_input_names.size());
    }
    ++steps_count;
  }

  size_t
  Counterexample::get_vars_count() const
  {
    return vars_state_names.size();
  }

  real_t
  Counterexample::get_var_value(size_t step,
                                const string& var_name) const
  {
    if (vars_state_names.count(var_name) != 0)
    {
      size_t idx = vars_state_names.find(var_name)->second;
      return vars_state_values.at(step).at(idx);
    }
    else
      throw logic_error("Unknown variable name for Counterexample.");
  }

  void
  Counterexample::set_var_value(size_t step,
                                const string& var_name,
                                real_t value)
  {
    if (vars_state_names.count(var_name) != 0)
    {
      size_t idx = vars_state_names.find(var_name)->second;
      vars_state_values.at(step).at(idx) = value;
    }
    else
      throw logic_error("Unknown variable name for Counterexample.");
  }

  size_t
  Counterexample::get_ivars_count() const
  {
    return vars_input_names.size();
  }

  real_t
  Counterexample::get_ivar_value(size_t step,
                                const string& var_name) const
  {
    if (vars_state_names.count(var_name) != 0)
    {
      size_t idx = vars_input_names.find(var_name)->second;
      return vars_input_values.at(step).at(idx);
    }
    else
      throw logic_error("Unknown variable name for Counterexample.");
  }

  void
  Counterexample::set_ivar_value(size_t step,
                                 const string& var_name,
                                 real_t value)
  {
    if (vars_input_names.count(var_name) != 0)
    {
      size_t idx = vars_input_names.find(var_name)->second;
      vars_input_values.at(step).at(idx) = value;
    }
    else
      throw logic_error("Unknown variable name for Counterexample.");
  }
}; // namespace mc_hybrid
