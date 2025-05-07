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

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _totxs;
  const unsigned int _N;
  const unsigned int _group;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
