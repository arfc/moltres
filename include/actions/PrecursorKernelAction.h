#ifndef PRECURSORKERNELACTION_H
#define PRECURSORKERNELACTION_H

#include "Action.h"

class PrecursorKernelAction: public Action
{
public:
  PrecursorKernelAction(const InputParameters & params);

  virtual void act();

protected:
  /// number of grains to create
  int _num_precursor_groups;

  /// base name for the order parameter variables
  std::string _var_name_base;

  int _num_groups;
};

template<>
InputParameters validParams<PrecursorKernelAction>();

#endif //PRECURSORKERNELACTION_H
