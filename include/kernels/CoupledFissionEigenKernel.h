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
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const VariableValue & _v;
  unsigned int _v_id;
  const MaterialProperty<Real> & _nu_f;
  const MaterialProperty<Real> & _sigma_f;
};

#endif //COUPLEDFISSIONEIGENKERNEL_H
