#pragma once

#include "AuxKernel.h"
#include "ScalarTransportBase.h"

class DelayedNeutronSourceAux : public AuxKernel, public ScalarTransportBase
{
public:
  DelayedNeutronSourceAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

  const MaterialProperty<std::vector<Real>> & _decay_constant;
  const unsigned int _num_precursor_groups;
  const VariableValue & _temp;
  std::vector<const VariableValue *> _pre_concs;
};
