#ifndef DIRACHX_H
#define DIRACHX_H

// Moose Includes
#include "DiracKernel.h"

// Forward Declarations
class DiracHX;

template <>
InputParameters validParams<DiracHX>();

class DiracHX : public DiracKernel
{
public:
  DiracHX(const InputParameters & parameters);

  virtual void addPoints() override;
  virtual Real computeQpResidual() override;

protected:
  const Real & _power;
  Point _point;
};

#endif // DIRACHX_H
