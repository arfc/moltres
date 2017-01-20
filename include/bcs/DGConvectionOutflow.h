#ifndef DGCONVECTIONOUTFLOW_H
#define DGCONVECTIONOUTFLOW_H

#include "IntegratedBC.h"

class DGConvectionOutflow;

template<>
InputParameters validParams<DGConvectionOutflow>();

class DGConvectionOutflow : public IntegratedBC
{
public:
  DGConvectionOutflow(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  RealVectorValue _velocity;
};

#endif //DGCONVECTIONOUTFLOW_H
