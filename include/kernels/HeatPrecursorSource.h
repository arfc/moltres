#ifndef HEATPRECURSORSOURCE_H
#define HEATPRECURSORSOURCE_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class HeatPrecursorSource;

template <>
InputParameters validParams<HeatPrecursorSource>();

class HeatPrecursorSource : public Kernel, public ScalarTransportBase
{
public:
  HeatPrecursorSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<std::vector<Real>> & _fisse;
  const MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;
  const MaterialProperty<std::vector<Real>> & _fissxs;
  const MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  unsigned int _num_groups;
  unsigned int _heat_group;
  Real _nt_scale;
  std::vector<Real> _decay_heat_frac;
  std::vector<Real> _decay_heat_const;
  const VariableValue & _temp;
  unsigned int _temp_id;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
};

#endif // HEATPRECURSORSOURCE_H