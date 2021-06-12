#pragma once

#include "IntegratedBC.h"

class MatDiffusionFluxBC : public IntegratedBC
{
public:
  MatDiffusionFluxBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  const MaterialProperty<Real> * _diff;
};
