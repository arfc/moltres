#include "ConservativeAdvectionNoBCBC.h"

registerMooseObject("MoltresApp", ConservativeAdvectionNoBCBC);

InputParameters
ConservativeAdvectionNoBCBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addRequiredParam<RealVectorValue>("velocity", "Velocity vector");
  return params;
}

ConservativeAdvectionNoBCBC::ConservativeAdvectionNoBCBC(const InputParameters & parameters)
  : IntegratedBC(parameters), _velocity(getParam<RealVectorValue>("velocity"))
{
}

Real
ConservativeAdvectionNoBCBC::computeQpResidual()
{
  return _u[_qp] * (_velocity * _test[_i][_qp] * _normals[_qp]);
}

Real
ConservativeAdvectionNoBCBC::computeQpJacobian()
{
  return _phi[_j][_qp] * (_velocity * _test[_i][_qp] * _normals[_qp]);
}
