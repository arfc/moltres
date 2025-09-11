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

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const unsigned int _group;
  std::vector<const Function *> _func;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
