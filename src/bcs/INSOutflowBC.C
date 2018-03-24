#include "INSOutflowBC.h"

registerMooseObject("MoltresApp", INSOutflowBC);

template <>
InputParameters
validParams<INSOutflowBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  // Coupled variables
  params.addRequiredCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", 0, "y-velocity"); // only required in 2D and 3D
  params.addCoupledVar("w", 0, "z-velocity"); // only required in 3D
  params.addRequiredCoupledVar("p", "pressure");

  // Required parameters
  params.addRequiredParam<Real>("mu", "dynamic viscosity");
  params.addRequiredParam<Real>("rho", "density");
  params.addRequiredParam<unsigned>(
      "component",
      "0,1,2 depending on if we are solving the x,y,z component of the momentum equation");
  params.addParam<bool>("integrate_p_by_parts",
                        true,
                        "Allows simulations to be run with pressure BC if set to false");

  return params;
}

INSOutflowBC::INSOutflowBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    // Coupled variables
    _u_vel(coupledValue("u")),
    _v_vel(coupledValue("v")),
    _w_vel(coupledValue("w")),
    _p(coupledValue("p")),

    // Gradients
    _grad_u_vel(coupledGradient("u")),
    _grad_v_vel(coupledGradient("v")),
    _grad_w_vel(coupledGradient("w")),
    _grad_p(coupledGradient("p")),

    // Variable numberings
    _u_vel_var_number(coupled("u")),
    _v_vel_var_number(coupled("v")),
    _w_vel_var_number(coupled("w")),
    _p_var_number(coupled("p")),

    // Required parameters
    _mu(getParam<Real>("mu")),
    _rho(getParam<Real>("rho")),
    _component(getParam<unsigned>("component")),
    _integrate_p_by_parts(getParam<bool>("integrate_p_by_parts"))

// Material properties
// _dynamic_viscosity(getMaterialProperty<Real>("dynamic_viscosity"))
{
}

Real
INSOutflowBC::computeQpResidual()
{
  // The pressure part, -p (div v) or (dp/dx_{component}) * test if not integrated by parts.
  Real pressure_part = 0.;
  if (_integrate_p_by_parts)
    pressure_part = _p[_qp] * _normals[_qp](_component) * _test[_i][_qp];

  // The component'th row (or col, it's symmetric) of the viscous stress tensor
  RealVectorValue tau_row;

  // Apply BC that the gradient of each velocity component in the normal direction is zero
  switch (_component)
  {
    case 0:
      tau_row(0) = _grad_u_vel[_qp](0);
      tau_row(1) = _grad_v_vel[_qp](0);
      tau_row(2) = _grad_w_vel[_qp](0);
      break;

    case 1:
      tau_row(0) = _grad_u_vel[_qp](1);
      tau_row(1) = _grad_v_vel[_qp](1);
      tau_row(2) = _grad_w_vel[_qp](1);
      break;

    case 2:
      tau_row(0) = _grad_u_vel[_qp](2);
      tau_row(1) = _grad_v_vel[_qp](2);
      tau_row(2) = _grad_w_vel[_qp](2);
      break;

    default:
      mooseError("Unrecognized _component requested.");
  }

  // The viscous part, tau : grad(v)
  Real viscous_part = -_mu * (tau_row * _normals[_qp] * _test[_i][_qp]);

  return pressure_part + viscous_part;
}

// Implement Jacobian

Real
INSOutflowBC::computeQpJacobian()
{
  return 0.;
}

Real
INSOutflowBC::computeQpOffDiagJacobian(unsigned /*jvar*/)
{
  return 0.;
}
