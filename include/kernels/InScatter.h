#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class InScatter : public Kernel, public ScalarTransportBase
{
public:
  InScatter(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _gtransfxs;
  const MaterialProperty<std::vector<Real>> & _d_gtransfxs_d_temp;
  unsigned int _group;
  unsigned int _num_groups;
  unsigned int _temp_id;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  bool _sss2_input;
};
