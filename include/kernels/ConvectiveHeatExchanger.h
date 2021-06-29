#pragma once

#include "Kernel.h"

class ConvectiveHeatExchanger : public Kernel
{
public:
  ConvectiveHeatExchanger(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const Real & _htc;
  Real _tref;
};
