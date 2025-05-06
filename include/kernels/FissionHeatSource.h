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

  const MaterialProperty<std::vector<Real>> & _fissxs;
  const MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  const MaterialProperty<std::vector<Real>> & _fisse;
  const MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;
  unsigned int _num_groups;
  const PostprocessorValue & _tot_fission_heat;
  const Real _power;
  const bool _has_heat_source;
  const VariableValue & _heat_source;
  const Real _gamma_frac;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
};
