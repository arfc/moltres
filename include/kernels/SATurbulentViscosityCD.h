#pragma once

#include "ADKernelStabilized.h"

/**
 * This class computes the residual and Jacobian contributions for the
 * Spalart-Allmaras equation crosswind stabilization
 */
class SATurbulentViscosityCD : public ADKernelStabilized
{
public:
  static InputParameters validParams();

  SATurbulentViscosityCD(const InputParameters & parameters);

protected:
  ADRealVectorValue computeQpStabilization() override;
  ADReal precomputeQpStrongResidual() override;

  const ADMaterialProperty<Real> & _tau_visc_cd;
  const ADVectorVariableValue & _velocity;
  const ADMaterialProperty<Real> & _visc_strong_residual;
};
