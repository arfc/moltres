#include "MatDiffusionFluxBC.h"

template<>
InputParameters validParams<MatDiffusionFluxBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<MaterialPropertyName>("prop_name", "the name of the material property we are going to use");

  return params;
}

MatDiffusionFluxBC::MatDiffusionFluxBC(const InputParameters & parameters) :
    IntegratedBC(parameters)
{
  _diff = &getMaterialProperty<Real>("prop_name");
}

Real
MatDiffusionFluxBC::computeQpResidual()
{
  return -_test[_i][_qp] * (*_diff)[_qp] * _grad_u[_qp] * _normals[_qp];
}

Real
MatDiffusionFluxBC::computeQpJacobian()
{
  return -_test[_i][_qp] * (*_diff)[_qp] * _grad_phi[_j][_qp] * _normals[_qp];
}
