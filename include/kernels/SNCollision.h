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

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _totxs;

  /// Neutron group number
  unsigned int _group;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
