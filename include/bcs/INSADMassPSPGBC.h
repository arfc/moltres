#pragma once

#include "ADIntegratedBC.h"

/**
 * This class removes INS PSPG contributions along the boundary.
 */
class INSADMassPSPGBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  INSADMassPSPGBC(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADMaterialProperty<Real> & _rho;
  const ADMaterialProperty<Real> & _tau;
  const ADMaterialProperty<RealVectorValue> & _momentum_strong_residual;
};
