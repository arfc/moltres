#pragma once

#include "ADVectorIntegratedBC.h"

/**
 * This class removes INS SUPG contributions along the boundary.
 */
class INSADMomentumSUPGBC : public ADVectorIntegratedBC
{
public:
  static InputParameters validParams();

  INSADMomentumSUPGBC(const InputParameters & parameters);

protected:
  ADRealVectorValue computeQpStabilization();
  ADRealVectorValue precomputeQpStrongResidual();
  void computeResidual() override;
  void computeResidualsForJacobian() override;
  virtual ADReal computeQpResidual() override final;

  const ADMaterialProperty<Real> & _rho;
  const ADMaterialProperty<Real> & _tau;
  const ADMaterialProperty<RealVectorValue> & _momentum_strong_residual;
};
