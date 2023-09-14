#pragma once

#include "ADIntegratedBC.h"

/**
 * This class removes SUPG contributions in the Spalart-Allmaras equation along the boundary.
 */
class SATurbulentViscositySUPGBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  SATurbulentViscositySUPGBC(const InputParameters & parameters);

protected:
  ADRealVectorValue computeQpStabilization();
  ADReal precomputeQpStrongResidual();
  void computeResidual() override;
  void computeResidualsForJacobian() override;
  virtual ADReal computeQpResidual() override final;

  const ADMaterialProperty<Real> & _tau_visc;
  const ADVectorVariableValue & _velocity;
  const ADMaterialProperty<Real> & _visc_strong_residual;
};
