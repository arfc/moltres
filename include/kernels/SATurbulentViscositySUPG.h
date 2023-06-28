#pragma once

#include "ADKernelSUPG.h"

/**
 * This class computes the residual and Jacobian contributions for
 * the Spalart-Allmaras equation stabilization
 */
class SATurbulentViscositySUPG : public ADKernelSUPG
{
public:
  static InputParameters validParams();

  SATurbulentViscositySUPG(const InputParameters & parameters);

protected:
  ADReal precomputeQpStrongResidual() override;
  
  const ADMaterialProperty<Real> & _visc_strong_residual;
};
