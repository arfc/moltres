#pragma once

#include "ArrayKernel.h"

class SNStreaming : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNStreaming(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _totxs;

  /// Neutron group number
  const unsigned int _group;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
