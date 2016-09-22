#ifndef SIGMAR_H
#define SIGMAR_H

#include "Kernel.h"
#include "MaterialProperty.h"

// Forward Declaration
class SigmaR;

template<>
InputParameters validParams<SigmaR>();


class SigmaR : public Kernel
{
public:
  SigmaR(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _sigma_r;
};

#endif //SIGMAR_H
