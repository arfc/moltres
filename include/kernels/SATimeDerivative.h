#pragma once

#include "ADTimeDerivative.h"

/**
 * This object computes the residual and Jacobian contributions for the time derivative term
 * in the turbulent viscosity equation of the Spalart-Allmaras model.
 */
class SATimeDerivative : public ADTimeDerivative
{
public:
  SATimeDerivative(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual ADReal precomputeQpResidual() override;

  const ADMaterialProperty<Real> & _time_strong_residual;
};
