#pragma once

#include "INSADMomentumViscous.h"

/**
 * This class computes the momentum equation residual and Jacobian
 * contributions for the turbulent viscous term of the incompressible
 * Navier-Stokes momentum equation.
 */
class INSADMomentumTurbulentViscous : public INSADMomentumViscous
{
public:
  static InputParameters validParams();

  INSADMomentumTurbulentViscous(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  /**
   * Computes the cartesian coordinate system viscous term
   */
  ADRealTensorValue qpViscousTerm();

  /**
   * Computes an additional contribution to the viscous term present of RZ coordinate systems
   * (assumes axisymmetric)
   */
  ADRealVectorValue qpAdditionalRZTerm();

  const ADVariableValue & _mu_tilde;
};
