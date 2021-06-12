#pragma once

#include "Action.h"

class VariableNotAMooseObjectAction : public Action
{
public:
  VariableNotAMooseObjectAction(const InputParameters & params);

  static InputParameters validParams();

protected:
  /**
   * Get the block ids from the input parameters
   * @return A set of block ids defined in the input
   */
  std::set<SubdomainID> getSubdomainIDs();

  /**
   * Add a variable
   * @param var_name The variable name
   */
  void addVariable(const std::string & var_name);
};
