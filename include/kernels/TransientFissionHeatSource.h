#ifndef TRANSIENTFISSIONEHEATSOURCE_H
#define TRANSIENTFISSIONEHEATSOURCE_H

#include "Kernel.h"

//Forward Declarations
class TransientFissionHeatSource;

template<>
InputParameters validParams<TransientFissionHeatSource>();

class TransientFissionHeatSource : public Kernel
{
public:
  TransientFissionHeatSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<std::vector<Real> > & _fissxs;
  const MaterialProperty<std::vector<Real> > & _d_fissxs_d_temp;
  const MaterialProperty<std::vector<Real> > & _fisse;
  const MaterialProperty<std::vector<Real> > & _d_fisse_d_temp;
  int _num_groups;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
};

#endif //TRANSIENTFISSIONEHEATSOURCE_H
