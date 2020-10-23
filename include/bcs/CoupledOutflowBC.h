#ifndef COUPLEDOUTFLOWBC_H
#define COUPLEDOUTFLOWBC_H

#include "IntegratedBC.h"

class CoupledOutflowBC;

template<>
InputParameters validParams<CoupledOutflowBC>();

class CoupledOutflowBC : public IntegratedBC
{
public:
  CoupledOutflowBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

private:
  const VariableValue & _vel_x;
  const VariableValue & _vel_y;
  const VariableValue & _vel_z;
};

#endif // COUPLEDOUTFLOWBC_H
