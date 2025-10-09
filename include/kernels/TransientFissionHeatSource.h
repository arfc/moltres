#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

/**
 * This class computes the residual and Jacobian contributions of the
 * fission heat source term in the temperature governing equation for
 * the time-dependent case.
 */
class TransientFissionHeatSource : public Kernel, public ScalarTransportBase
{
public:
  TransientFissionHeatSource(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _fissxs;
  const MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  const MaterialProperty<std::vector<Real>> & _fisse;
  const MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;

  /// Number of neutron groups
  unsigned int _num_groups;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  /// Group flux variable IDs
  std::vector<unsigned int> _flux_ids;

  /// Neutron flux scaling factor
  Real _nt_scale;

  /// Whether to account for decay heat
  bool _account_decay_heat;

  /// Number of decay heat groups
  unsigned int _num_heat_groups;

  /// Decay heat group fractions
  std::vector<Real> _decay_heat_frac;

  /// Decay heat group decay constants
  std::vector<Real> _decay_heat_const;

  /// Whether a heat source auxvariable is provided
  const bool _has_heat_source;

  /// Heat source auxvariable
  const VariableValue & _heat_source;

  /// Gamma heating fraction in non-fissile regions
  const Real _gamma_frac;

  /// Decay heat variables
  std::vector<const VariableValue *> _heat_concs;

  /// Decay heat variable IDs
  std::vector<unsigned int> _heat_ids;
};
