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

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _decay_constant;

  /// Number of precursor groups
  const unsigned int _num_precursor_groups;

  /// Temperature variable
  const VariableValue & _temp;

  /// Precursor variables
  std::vector<const VariableValue *> _pre_concs;
};
