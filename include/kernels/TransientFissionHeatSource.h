#ifndef TRANSIENTFISSIONEHEATSOURCE_H
#define TRANSIENTFISSIONEHEATSOURCE_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class TransientFissionHeatSource;

template <>
InputParameters validParams<TransientFissionHeatSource>();

class TransientFissionHeatSource : public Kernel, public ScalarTransportBase
{
public:
  TransientFissionHeatSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

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

#endif // TRANSIENTFISSIONEHEATSOURCE_H
