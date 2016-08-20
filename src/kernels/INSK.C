#include "INSK.h"

template<>
InputParameters validParams<INSK>()
{
  InputParameters params = validParams<Kernel>();

  // Coupled variables
  params.addRequiredCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", 0, "y-velocity"); // only required in 2D and 3D
  params.addCoupledVar("w", 0, "z-velocity"); // only required in 3D
  params.addRequiredCoupledVar("epsilon", "turbulent dissipation");

  // Required parameters
  params.addRequiredParam<Real>("mu", "dynamic viscosity");
  params.addRequiredParam<Real>("rho", "density");
  return params;
}



INSK::INSK(const InputParameters & parameters) :
  Kernel(parameters),

  // Coupled variables
  _u_vel(coupledValue("u")),
  _v_vel(coupledValue("v")),
  _w_vel(coupledValue("w")),
  _epsilon(coupledValue("epsilon")),

  // Gradients
  _grad_u_vel(coupledGradient("u")),
  _grad_v_vel(coupledGradient("v")),
  _grad_w_vel(coupledGradient("w")),
  _grad_epsilon(coupledGradient("epsilon")),

  // Variable numberings
  _u_vel_var_number(coupled("u")),
  _v_vel_var_number(coupled("v")),
  _w_vel_var_number(coupled("w")),
  _epsilon_var_number(coupled("epsilon")),

  // Required parameters
  _mu(getParam<Real>("mu")),
  _rho(getParam<Real>("rho")),
  _gravity(getParam<RealVectorValue>("gravity")),
  _Cmu(0.09),
  _sigk(1.00),
  _sigeps(1.30),
  _C1eps(1.44),
  _C2eps(1.92)

  // Material properties
  // _dynamic_viscosity(getMaterialProperty<Real>("dynamic_viscosity"))
{
}



Real INSK::computeQpResidual()
{
  // The convection part, rho * (u.grad) * u_component * v.
  // Note: _grad_u is the gradient of the _component entry of the velocity vector.
  Real convective_part = _rho *
    (_u_vel[_qp]*_grad_u[_qp](0) +
     _v_vel[_qp]*_grad_u[_qp](1) +
     _w_vel[_qp]*_grad_u[_qp](2)) * _test[_i][_qp];

  // The diffusive part
  Real eddy_visc = _rho * _Cmu * std::pow(_u[_qp], 2) * _epsilon[_qp];
  Real diffusive_part = _grad_test[_i][_qp] * (_mu + eddy_visc / _sigk) * _grad_u[_qp];

  // Turbulent dissipative sink part
  Real dissipative_part = _test[_i][_qp] * _rho * _epsilon[_qp];

  // Source term from velocity gradients
  Real strain_tensor_double_dot_product;
  RealVectorValue vel(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
  // Make vector of vectors or something here; I need a 3x3 matrix for accesing the j-derivative of the ith component of the velocity 
  Real source_term = -_test[_i][_qp] * 2. * strain_tensor_double_dot_product;

  return convective_part + diffusive_part + dissipative_part;
}




Real INSK::computeQpJacobian()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  // Convective part
  Real convective_part = _rho * ((U*_grad_phi[_j][_qp]) + _phi[_j][_qp]*_grad_u[_qp](_component)) * _test[_i][_qp];

  // Viscous part, Stokes/Laplacian version
  // Real viscous_part = _mu * (_grad_phi[_j][_qp] * _grad_test[_i][_qp]);

  // Viscous part, full stress tensor.  The extra contribution comes from the "2"
  // on the diagonal of the viscous stress tensor.
  Real viscous_part = _mu * (_grad_phi[_j][_qp]             * _grad_test[_i][_qp] +
                             _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](_component));

  return convective_part + viscous_part;
}




Real INSK::computeQpOffDiagJacobian(unsigned jvar)
{
  // In Stokes/Laplacian version, off-diag Jacobian entries wrt u,v,w are zero
  if (jvar == _u_vel_var_number)
  {
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](0) * _test[_i][_qp];
    Real viscous_part = _mu * _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](0);

    return convective_part + viscous_part;
  }

  else if (jvar == _v_vel_var_number)
  {
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](1) * _test[_i][_qp];
    Real viscous_part = _mu * _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](1);

    return convective_part + viscous_part;
  }

  else if (jvar == _w_vel_var_number)
  {
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](2) * _test[_i][_qp];
    Real viscous_part = _mu * _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](2);

    return convective_part + viscous_part;
  }

  else if (jvar == _p_var_number)
  {
    if (_integrate_p_by_parts)
      return -_phi[_j][_qp] * _grad_test[_i][_qp](_component);
    else
      return _grad_phi[_j][_qp](_component) * _test[_i][_qp];
  }

  else if (jvar == _k_var_number)
    return _test[_i][_qp] * 2. / 3. * _rho * _grad_phi[_j][_qp](_component);


  else
    return 0;
}
