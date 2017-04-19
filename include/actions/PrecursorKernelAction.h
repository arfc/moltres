#ifndef PRECURSORKERNELACTION_H
#define PRECURSORKERNELACTION_H

#include "AddVariableAction.h"

class PrecursorKernelAction : public AddVariableAction
{
public:
  PrecursorKernelAction(const InputParameters & params);

  virtual void act();

protected:
  /// number of grains to create
  unsigned int _num_precursor_groups;

  /// base name for the order parameter variables
  std::string _var_name_base;

  unsigned int _num_groups;
};

template <>
InputParameters validParams<PrecursorKernelAction>();

#endif // PRECURSORKERNELACTION_H
