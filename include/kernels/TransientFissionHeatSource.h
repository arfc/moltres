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

  // Material properties
  const MaterialProperty<std::vector<Real>> & _fissxs;
  const MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  const MaterialProperty<std::vector<Real>> & _fisse;
  const MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;

  unsigned int _num_groups;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  Real _nt_scale;
  bool _account_decay_heat;
  unsigned int _num_heat_groups;
  std::vector<Real> _decay_heat_frac;
  std::vector<Real> _decay_heat_const;
  std::vector<const VariableValue *> _heat_concs;
  std::vector<unsigned int> _heat_ids;
};
