#include "MatINSTemperatureNoBCBC.h"

template<>
InputParameters validParams<MatINSTemperatureNoBCBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  // Required parameters
  params.addRequiredParam<MaterialPropertyName>("k", "The name of the thermal conductivity material property");

  return params;
}



MatINSTemperatureNoBCBC::MatINSTemperatureNoBCBC(const InputParameters & parameters) :
  IntegratedBC(parameters),
  // Required parameters
  _k(getMaterialProperty<Real>("k"))
{
}



Real MatINSTemperatureNoBCBC::computeQpResidual()
{
  // k * (grad_T.n) * test
  return _k[_qp] * _grad_u[_qp] * _normals[_qp] * _test[_i][_qp];
}




Real MatINSTemperatureNoBCBC::computeQpJacobian()
{
  return _k[_qp] * (_grad_phi[_j][_qp]*_normals[_qp]) * _test[_i][_qp];
}




Real MatINSTemperatureNoBCBC::computeQpOffDiagJacobian(unsigned /*jvar*/)
{
  // off-diagonal derivatives are all zero.
  return 0.;
}
