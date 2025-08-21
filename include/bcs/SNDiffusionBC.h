#pragma once

#include "ArrayIntegratedBC.h"

class SNDiffusionBC : public ArrayIntegratedBC
{
public:
  static InputParameters validParams();

  SNDiffusionBC(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;

  unsigned int _N;
  const MaterialProperty<std::vector<Real>> & _diffcoef;
  unsigned int _group;
  const VariableValue & _diff_flux;
  const VariableGradient & _grad_diff_flux;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
