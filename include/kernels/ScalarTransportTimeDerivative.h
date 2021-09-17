#pragma once

#include "TimeKernel.h"
#include "ScalarTransportBase.h"

class ScalarTransportTimeDerivative : public TimeKernel, public ScalarTransportBase
{
public:
  ScalarTransportTimeDerivative(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  bool _lumping;
  Real _conc_scaling;
};
