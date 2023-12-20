#pragma once

#include "SideIntegralVariablePostprocessor.h"

/**
 * This postprocessor computes the neutron leakage rate of a given neutron group
 * along a boundary
 */
class NeutronLeakage : public SideIntegralVariablePostprocessor
{
public:
  NeutronLeakage(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  Real computeQpIntegral() override;

  const MaterialProperty<std::vector<Real>> & _diffcoef;
  unsigned int _group;
};
