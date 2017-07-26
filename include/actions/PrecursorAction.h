#ifndef PRECURSORACTION_H
#define PRECURSORACTION_H

#include "AddVariableAction.h"

class PrecursorAction : public AddVariableAction
{
public:
  PrecursorAction(const InputParameters & params);

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
InputParameters validParams<PrecursorAction>();

#endif // PRECURSORACTION_H
