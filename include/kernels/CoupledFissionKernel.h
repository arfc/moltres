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

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  const MaterialProperty<std::vector<Real>> & _chi_t;
  const MaterialProperty<std::vector<Real>> & _chi_p;
  const MaterialProperty<std::vector<Real>> & _d_chi_t_d_temp;
  const MaterialProperty<std::vector<Real>> & _d_chi_p_d_temp;
  const MaterialProperty<Real> & _beta;
  const MaterialProperty<Real> & _d_beta_d_temp;

  /// Neutron group number
  const unsigned int _group;

  /// Number of neutron groups
  const unsigned int _num_groups;

  /// Temperature variable ID
  const unsigned int _temp_id;

  /// Temperature variable
  const VariableValue & _temp;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  /// Group flux variable IDs
  std::vector<unsigned int> _flux_ids;

  /// Whether to account for delayed neutron precursors
  const bool _account_delayed;

  /// Eigenvalue scaling factor
  const PostprocessorValue & _eigenvalue_scaling;
};
