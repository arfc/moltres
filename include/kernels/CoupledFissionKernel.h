#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

/**
 * Computes fission source of neutrons without normalizing by
 * \f$ 1/k \f$. Note that this kernel is meant for transients.
 */
class CoupledFissionKernel : public Kernel, public ScalarTransportBase
{
public:
  CoupledFissionKernel(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  const MaterialProperty<std::vector<Real>> & _chi_t;
  const MaterialProperty<std::vector<Real>> & _chi_p;
  const MaterialProperty<std::vector<Real>> & _d_chi_t_d_temp;
  const MaterialProperty<std::vector<Real>> & _d_chi_p_d_temp;
  const MaterialProperty<Real> & _beta;
  const MaterialProperty<Real> & _d_beta_d_temp;
  unsigned int _group;
  unsigned int _num_groups;
  unsigned int _temp_id;
  const VariableValue & _temp;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  bool _account_delayed;
  Real _eigenvalue_scaling;
};
