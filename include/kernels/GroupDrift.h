#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class GroupDrift : public Kernel, public ScalarTransportBase
{
public:
  GroupDrift(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _diffcoef;
  const MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;
  const MaterialProperty<std::vector<Real>> & _totxs;
  const MaterialProperty<std::vector<Real>> & _d_totxs_d_temp;
  const MaterialProperty<std::vector<Real>> & _scatter;
  const MaterialProperty<std::vector<Real>> & _d_scatter_d_temp;
  const unsigned int _N;
  const unsigned int _group;
  const unsigned int _num_groups;
  const unsigned int _temp_id;
  std::vector<const ArrayVariableValue *> _group_fluxes;
  std::vector<const ArrayVariableGradient *> _grad_group_fluxes;
  std::vector<unsigned int> _flux_ids;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
