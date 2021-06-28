#pragma once

#include "Kernel.h"

class ManuHX : public Kernel
{
public:
  ManuHX(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  Real _htc;
  Real _tref;
};
