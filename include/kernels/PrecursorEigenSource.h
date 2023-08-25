#pragma once

#include "EigenKernel.h"
#include "ScalarTransportBase.h"

/**
 * This object computes the residual and Jacobian contribution for the precursor source term in the
 * delayed neutron precursor equation for k-eigenvalue problems.
 */
class PrecursorEigenSource : public EigenKernel, public ScalarTransportBase
{
public:
  PrecursorEigenSource(const InputParameters & parameters);

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
