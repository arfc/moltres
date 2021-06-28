#include "DiffusionNoBCBC.h"

registerMooseObject("MoltresApp", DiffusionNoBCBC);

InputParameters
DiffusionNoBCBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription("The Laplacian operator ($-\\nabla \\cdot \\nabla u$), with the weak "
                             "form of $(\\nabla \\phi_i, \\nabla u_h)$.");
  return params;
}

DiffusionNoBCBC::DiffusionNoBCBC(const InputParameters & parameters) : IntegratedBC(parameters) {}

Real
DiffusionNoBCBC::computeQpResidual()
{
  return _grad_u[_qp] * -_test[_i][_qp] * _normals[_qp];
}

Real
DiffusionNoBCBC::computeQpJacobian()
{
  return _grad_phi[_j][_qp] * -_test[_i][_qp] * _normals[_qp];
}
