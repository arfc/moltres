#include "CoupledScalarAdvectionNoBCBC.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<CoupledScalarAdvectionNoBCBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  // Coupled variables
  params.addCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", "y-velocity");
  params.addCoupledVar("w", "z-velocity");
  params.addParam<Real>("u_def", 0, "Allows user to specify constant value for u component of velocity.");
  params.addParam<Real>("v_def", 0, "Allows user to specify constant value for v component of velocity.");
  params.addParam<Real>("w_def", 0, "Allows user to specify constant value for w component of velocity.");
  return params;
}



CoupledScalarAdvectionNoBCBC::CoupledScalarAdvectionNoBCBC(const InputParameters & parameters) :
    IntegratedBC(parameters),

    // Coupled variables
    _u_vel(isCoupled("u") ? coupledValue("u") : _u_def),
    _v_vel(isCoupled("v") ? coupledValue("v") : _v_def),
    _w_vel(isCoupled("w") ? coupledValue("w") : _w_def),

    // Variable numberings
    _u_vel_var_number(coupled("u")),
    _v_vel_var_number(coupled("v")),
    _w_vel_var_number(coupled("w"))
{
  if (!(isCoupled("u")))
    _u_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("u_def")));
  if (!(isCoupled("v")))
    _v_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("v_def")));
  if (!(isCoupled("w")))
    _w_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("w_def")));
}



Real CoupledScalarAdvectionNoBCBC::computeQpResidual()
{
  return (_normals[_qp](0) * _u_vel[_qp] +
           _normals[_qp](1) * _v_vel[_qp] +
           _normals[_qp](2) * _w_vel[_qp]) * _u[_qp];
}




Real CoupledScalarAdvectionNoBCBC::computeQpJacobian()
{
  return (_normals[_qp](0) * _u_vel[_qp] +
           _normals[_qp](1) * _v_vel[_qp] +
           _normals[_qp](2) * _w_vel[_qp]) * _phi[_j][_qp];
}




Real CoupledScalarAdvectionNoBCBC::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _u_vel_var_number)
    return _normals[_qp](0) * _phi[_j][_qp] * _u[_qp];

  else if (jvar == _v_vel_var_number)
    return _normals[_qp](1) * _phi[_j][_qp] * _u[_qp];

  else if (jvar == _w_vel_var_number)
    return _normals[_qp](2) * _phi[_j][_qp] * _u[_qp];

  else
    return 0;
}
