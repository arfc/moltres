#pragma once

#include "SideIntegralPostprocessor.h"
#include "ScalarTransportBase.h"

/**
 * This postprocess computes the total neutron leakage rate along a boundary for all neutron
 * groups
 */
class TotalNeutronLeakage : public SideIntegralPostprocessor, public ScalarTransportBase
{
public:
  TotalNeutronLeakage(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  Real computeQpIntegral() override;

  std::vector<MooseVariableFEBase *> _vars;
  const MaterialProperty<std::vector<Real>> & _diffcoef;
  int _num_groups;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<const VariableGradient *> _grad_group_fluxes;
};
