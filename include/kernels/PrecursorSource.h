#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class PrecursorSource : public Kernel, public ScalarTransportBase
{
public:
  PrecursorSource(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  unsigned int _num_groups;
  const MaterialProperty<std::vector<Real>> & _beta_eff;
  const MaterialProperty<std::vector<Real>> & _d_beta_eff_d_temp;
  unsigned int _precursor_group;
  const VariableValue & _temp;
  unsigned int _temp_id;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  Real _prec_scale;
};
