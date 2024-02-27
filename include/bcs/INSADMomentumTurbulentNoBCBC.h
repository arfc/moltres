#pragma once

#include "INSADMomentumNoBCBC.h"

/**
 * This class implements the "No BC" boundary condition with additional contributions from
 * turbulent viscosity.
 */
class INSADMomentumTurbulentNoBCBC : public INSADMomentumNoBCBC
{
public:
  static InputParameters validParams();

  INSADMomentumTurbulentNoBCBC(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADVariableValue & _mu_tilde;
};
