#pragma once

#include "Kernel.h"

/**
 * This kernel will likely only be used with k-eigenvalue calculation mode
 * with neutrons only, since the power is normalized in this case.
 */
class FissionHeatSource : public Kernel
{
public:
  FissionHeatSource(const InputParameters & parameters);

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

  /// Total fission heat postprocessor
  const PostprocessorValue & _tot_fission_heat;

  /// User-defined power output
  const Real _power;

  /// Whether a heat source auxvariable is provided
  const bool _has_heat_source;

  /// Heat source variable
  const VariableValue & _heat_source;

  /// Gamma heating fraction in non-fissile regions
  const Real _gamma_frac;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  /// Group flux variable IDs
  std::vector<unsigned int> _flux_ids;
};
