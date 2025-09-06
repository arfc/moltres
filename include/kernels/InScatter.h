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
  const unsigned int _group;
  const unsigned int _num_groups;
  const unsigned int _temp_id;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  const bool _sss2_input;
  std::vector<int> _idx;
};
