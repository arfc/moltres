#include "CoupledScalarAdvection.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<CoupledScalarAdvection>()
{
  InputParameters params = validParams<Kernel>();

  // Coupled variables
  params.addCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", "y-velocity"); // only required in 2D and 3D
  params.addCoupledVar("w", "z-velocity"); // only required in 3D
  return params;
}



CoupledScalarAdvection::CoupledScalarAdvection(const InputParameters & parameters) :
  Kernel(parameters),

  // Coupled variables
  _u_vel(coupledValue("u")),
  _v_vel(_mesh.dimension() >= 2 ? coupledValue("v") : _zero),
  _w_vel(_mesh.dimension() == 3 ? coupledValue("w") : _zero),

  // Variable numberings
  _u_vel_var_number(coupled("u")),
  _v_vel_var_number(_mesh.dimension() >= 2 ? coupled("v") : libMesh::invalid_uint),
  _w_vel_var_number(_mesh.dimension() == 3 ? coupled("w") : libMesh::invalid_uint)
{
}



Real CoupledScalarAdvection::computeQpResidual()
{
  return -(_grad_test[_i][_qp](0) * _u_vel[_qp] +
           _grad_test[_i][_qp](1) * _v_vel[_qp] +
           _grad_test[_i][_qp](2) * _w_vel[_qp]) * _u[_qp];
}




Real CoupledScalarAdvection::computeQpJacobian()
{
  return -(_grad_test[_i][_qp](0) * _u_vel[_qp] +
           _grad_test[_i][_qp](1) * _v_vel[_qp] +
           _grad_test[_i][_qp](2) * _w_vel[_qp]) * _phi[_j][_qp];
}




Real CoupledScalarAdvection::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _u_vel_var_number)
    return -_grad_test[_i][_qp](0) * _phi[_j][_qp] * _u[_qp];

  else if (jvar == _v_vel_var_number)
    return -_grad_test[_i][_qp](1) * _phi[_j][_qp] * _u[_qp];

  else if (jvar == _w_vel_var_number)
    return -_grad_test[_i][_qp](2) * _phi[_j][_qp] * _u[_qp];

  else
    return 0;
}
