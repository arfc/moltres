#ifndef SELFFISSIONEIGENKERNEL_H
#define SELFFISSIONEIGENKERNEL_H

#include "EigenKernel.h"

// Forward Declarations
class SelfFissionEigenKernel;

template <>
InputParameters validParams<SelfFissionEigenKernel>();

class SelfFissionEigenKernel : public EigenKernel
{
public:
  SelfFissionEigenKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _nu_f;
  const MaterialProperty<Real> & _sigma_f;
};

#endif // SELFFISSIONEIGENKERNEL_H
