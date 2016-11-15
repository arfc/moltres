#ifndef NTACTION_H
#define NTACTION_H

#include "AddVariableAction.h"

class NtAction: public AddVariableAction
{
public:
  NtAction(const InputParameters & params);

  virtual void act();

protected:
  /// number of grains to create
  int _num_precursor_groups;

  /// base name for the order parameter variables
  std::string _var_name_base;

  int _num_groups;
};

template<>
InputParameters validParams<NtAction>();

#endif //NTACTION_H
