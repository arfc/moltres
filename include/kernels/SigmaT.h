#ifndef SIGMAT_H
#define SIGMAT_H

#include "Kernel.h"
#include "MaterialProperty.h"

// Forward Declaration
class SigmaT;

template<>
InputParameters validParams<SigmaT>();


class SigmaT : public Kernel
{
public:
  SigmaT(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _sigma_t;
};

#endif //SIGMAT_H
