#pragma once

#include "AuxKernel.h"
#include "ScalarTransportBase.h"

class NeutronSourceAux : public AuxKernel, public ScalarTransportBase
{
public:
  NeutronSourceAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

  const MaterialProperty<std::vector<Real>> & _nsf;
  unsigned int _num_groups;
  const VariableValue & _temp;
  std::vector<const VariableValue *> _group_fluxes;
};
