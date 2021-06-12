#include "CoupledOutflowBC.h"

registerMooseObject("MoltresApp", CoupledOutflowBC);

InputParameters
CoupledOutflowBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addRequiredCoupledVar("uvel", "x direction velocity");
  params.addCoupledVar("vvel", 0, "y direction velocity");
  params.addCoupledVar("wvel", 0, "z direction velocity");
  params.addClassDescription("DG outflow for convection on Navier-Stokes velocity variables");
  return params;
}

CoupledOutflowBC::CoupledOutflowBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    // velocity variables
    _vel_x(coupledValue("uvel")),
    _vel_y(coupledValue("vvel")),
    _vel_z(coupledValue("wvel"))
{
}

Real
CoupledOutflowBC::computeQpResidual()
{
  Real r = 0;

  Real vdotn = _vel_x[_qp] * _normals[_qp](0) + _vel_y[_qp] * _normals[_qp](1) +
               _vel_z[_qp] * _normals[_qp](2);

  if (vdotn >= 0)
    r += vdotn * _u[_qp] * _test[_i][_qp];

  return r;
}

Real
CoupledOutflowBC::computeQpJacobian()
{
  Real r = 0;

  Real vdotn = _vel_x[_qp] * _normals[_qp](0) + _vel_y[_qp] * _normals[_qp](1) +
               _vel_z[_qp] * _normals[_qp](2);

  if (vdotn >= 0)
    r += vdotn * _phi[_j][_qp] * _test[_i][_qp];

  return r;
}
