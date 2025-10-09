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

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  const MaterialProperty<std::vector<Real>> & _beta_eff;
  const MaterialProperty<std::vector<Real>> & _d_beta_eff_d_temp;

  /// Number of neutron groups
  unsigned int _num_groups;

  /// Precursor group number
  unsigned int _precursor_group;

  /// Temperature variable
  const VariableValue & _temp;

  /// Temperature variable ID
  unsigned int _temp_id;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  /// Group flux variable IDs
  std::vector<unsigned int> _flux_ids;

  /// Precursor scaling factor
  Real _prec_scale;

  /// Eigenvalue scaling factor
  const PostprocessorValue & _eigenvalue_scaling;

  /// Whether a neutron source auxvariable is provided
  const bool _has_neutron_source;

  /// Neutron source variable
  const VariableValue & _neutron_source;
};
