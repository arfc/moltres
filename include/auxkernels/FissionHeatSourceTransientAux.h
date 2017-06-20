#ifndef FISSIONHEATSOURCETRANSIENTAUX_H
#define FISSIONHEATSOURCETRANSIENTAUX_H

#include "AuxKernel.h"

// Forward Declarations
class FissionHeatSourceTransientAux;

template <>
InputParameters validParams<FissionHeatSourceTransientAux>();

class FissionHeatSourceTransientAux : public AuxKernel
{
public:
  FissionHeatSourceTransientAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const MaterialProperty<std::vector<Real>> & _fissxs;
  const MaterialProperty<std::vector<Real>> & _fisse;
  unsigned int _num_groups;
  std::vector<const VariableValue *> _group_fluxes;
};

#endif // FISSIONHEATSOURCETRANSIENTAUX_H
