#ifndef PRECURSORKERNELACTION_H
#define PRECURSORKERNELACTION_H

#include "AddVariableAction.h"

class PrecursorKernelAction : public AddVariableAction
{
public:
  PrecursorKernelAction(const InputParameters & params);

  virtual void act();

protected:
  /// number of precursor groups
  unsigned int _num_precursor_groups;

  /// base name for neutron variables
  std::string _var_name_base;

  /// number of energy groups
  unsigned int _num_groups;

  /// optional object name suffix
  std::string _object_suffix;
};

template <>
InputParameters validParams<PrecursorKernelAction>();

#endif // PRECURSORKERNELACTION_H
