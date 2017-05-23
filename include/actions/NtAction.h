#ifndef NTACTION_H
#define NTACTION_H

#include "AddVariableAction.h"

class NtAction : public AddVariableAction
{
public:
  NtAction(const InputParameters & params);

  virtual void act();

protected:
  /// number of precursor groups
  unsigned int _num_precursor_groups;

  /// base name for neutron variables
  std::string _var_name_base;

  /// number of energy groups
  unsigned int _num_groups;
};

template <>
InputParameters validParams<NtAction>();

#endif // NTACTION_H
