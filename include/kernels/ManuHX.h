#ifndef MANUHX_H
#define MANUHX_H

#include "Kernel.h"

// Forward Declarations
class ManuHX;

template <>
InputParameters validParams<ManuHX>();

class ManuHX : public Kernel
{
public:
  ManuHX(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  Real _htc;
  Real _tref;
};

#endif // MANUHX_H
