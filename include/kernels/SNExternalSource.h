#pragma once

#include "ArrayKernel.h"

class Function;

class SNExternalSource : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNExternalSource(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _tau_sn;

  /// Neutron group number
  const unsigned int _group;

  /// External source function
  std::vector<const Function *> _func;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
