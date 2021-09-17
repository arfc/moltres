#pragma once

#include "Kernel.h"

class GammaHeatSource : public Kernel
{
public:
  GammaHeatSource(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const PostprocessorValue & _average_fission_heat;
  const Function & _gamma;
};
