#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class PrecursorDecay : public Kernel, public ScalarTransportBase
{
public:
  PrecursorDecay(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _decay_constant;
  const MaterialProperty<std::vector<Real>> & _d_decay_constant_d_temp;
  unsigned int _precursor_group;
  unsigned int _temp_id;
  const VariableValue & _temp;
  Real _prec_scale;
};
