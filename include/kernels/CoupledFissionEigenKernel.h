#ifndef COUPLEDFISSIONEIGENKERNEL_H
#define COUPLEDFISSIONEIGENKERNEL_H

#include "EigenKernel.h"

//Forward Declarations
class CoupledFissionEigenKernel;

template<>
InputParameters validParams<CoupledFissionEigenKernel>();

class CoupledFissionEigenKernel : public EigenKernel
{
public:
  CoupledFissionEigenKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<std::vector<Real> > & _nsf;
  const MaterialProperty<std::vector<Real> > & _d_nsf_d_temp;
  const MaterialProperty<std::vector<Real> > & _chi;
  const MaterialProperty<std::vector<Real> > & _d_chi_d_temp;
  int _group;
  int _num_groups;
  unsigned int _temp_id;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
};

#endif //COUPLEDFISSIONEIGENKERNEL_H
