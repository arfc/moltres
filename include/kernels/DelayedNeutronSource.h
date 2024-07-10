#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class DelayedNeutronSource : public Kernel, public ScalarTransportBase
{
public:
  DelayedNeutronSource(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _decay_constant;
  const MaterialProperty<std::vector<Real>> & _d_decay_constant_d_temp;
  const unsigned int _group;
  const MaterialProperty<std::vector<Real>> &  _chi_d;

  const unsigned int _num_precursor_groups;
  const unsigned int _temp_id;
  const VariableValue & _temp;
  std::vector<const VariableValue *> _pre_concs;
  std::vector<unsigned int> _pre_ids;
};
