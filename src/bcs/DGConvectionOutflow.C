#include "DGConvectionOutflow.h"

template<>
InputParameters validParams<DGConvectionOutflow>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<RealVectorValue>("velocity", "Velocity vector");
  params.addClassDescription("DG upwinding for the convection");
  return params;
}

DGConvectionOutflow::DGConvectionOutflow(const InputParameters & parameters) :
    IntegratedBC(parameters),
    _velocity(getParam<RealVectorValue>("velocity"))
{
}

Real
DGConvectionOutflow::computeQpResidual()
{
  Real r = 0;

  Real vdotn = _velocity * _normals[_qp];

  if (vdotn >= 0)
    r += vdotn * _u[_qp] * _test[_i][_qp];

  return r;
}

Real
DGConvectionOutflow::computeQpJacobian()
{
  Real r = 0;

  Real vdotn = _velocity * _normals[_qp];

  if (vdotn >= 0)
    r += vdotn * _phi[_j][_qp] * _test[_i][_qp];

  return r;
}
