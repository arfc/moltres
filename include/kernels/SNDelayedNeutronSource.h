#pragma once

#include "ArrayKernel.h"
#include "Eigen/Core"

class SNDelayedNeutronSource : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNDelayedNeutronSource(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;

  /// Level-symmetric quadrature normalization factor for isotropic source
  const Real _ls_norm_factor = 0.125;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _decay_constant;
  const MaterialProperty<std::vector<Real>> & _chi_d;

  /// Neutron group number
  const unsigned int _group;

  /// Delayed neutron source auxvariable
  const VariableValue & _delayed_source;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
