#ifndef CONSERVATIVEADVECTIONOBCBC_H
#define CONSERVATIVEADVECTIONOBCBC_H

#include "IntegratedBC.h"

// Forward Declaration
class ConservativeAdvectionNoBCBC;

template <>
InputParameters validParams<ConservativeAdvectionNoBCBC>();

class ConservativeAdvectionNoBCBC : public IntegratedBC
{
public:
  ConservativeAdvectionNoBCBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  RealVectorValue _velocity;
};

#endif // CONSERVATIVEADVECTIONOBCBC_H
