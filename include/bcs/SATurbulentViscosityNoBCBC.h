#pragma once

#include "ADVectorIntegratedBC.h"

/**
 * This class implements the "No BC" boundary condition of the Spalart-Allmaras equation
 */
class SATurbulentViscosityNoBCBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  SATurbulentViscosityNoBCBC(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADMaterialProperty<Real> & _mu;
};
