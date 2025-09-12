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

  const Real _ls_norm_factor = 0.125;
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _decay_constant;
  const MaterialProperty<std::vector<Real>> & _chi_d;
  const unsigned int _group;
  const VariableValue & _delayed_source;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
