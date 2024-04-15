#pragma once

#include "ArrayKernel.h"

class SNCollision : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNCollision(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;

  const MaterialProperty<std::vector<Real>> & _totxs;
  unsigned int _N;
  unsigned int _group;
  RealEigenVector _weights;
};
