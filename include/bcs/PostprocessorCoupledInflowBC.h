#ifndef POSTPROCESSORCOUPLEDINFLOWBC_H
#define POSTPROCESSORCOUPLEDINFLOWBC_H

#include "IntegratedBC.h"

class PostprocessorCoupledInflowBC;

template <>
InputParameters validParams<PostprocessorCoupledInflowBC>();

class PostprocessorCoupledInflowBC : public IntegratedBC
{
public:
  PostprocessorCoupledInflowBC(const InputParameters & parameters);

protected:
  const PostprocessorValue & _pp_value;
  const Real & _scale;
  const Real & _offset;
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

private:
  const VariableValue & _vel_x;
  const VariableValue & _vel_y;
  const VariableValue & _vel_z;
};

#endif // POSTPROCESSORCOUPLEDINFLOWBC_H
