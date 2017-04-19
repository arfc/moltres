#include "INSK.h"

template <>
InputParameters
validParams<INSK>()
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

INSK::INSK(const InputParameters & parameters)
  : Kernel(parameters),

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
    _Cmu(0.09),
    _sigk(1.00),
    _sigeps(1.30),
    _C1eps(1.44),
    _C2eps(1.92)

// Material properties
// _dynamic_viscosity(getMaterialProperty<Real>("dynamic_viscosity"))
{
}

Real
INSK::computeQpResidual()
{
  // The convection part
  Real convective_part = _rho * (_u_vel[_qp] * _grad_u[_qp](0) + _v_vel[_qp] * _grad_u[_qp](1) +
                                 _w_vel[_qp] * _grad_u[_qp](2)) *
                         _test[_i][_qp];

  // The diffusive part
  Real eddy_visc = _rho * _Cmu * std::pow(_u[_qp], 2) / _epsilon[_qp];
  Real diffusive_part = _grad_test[_i][_qp] * (_mu + eddy_visc / _sigk) * _grad_u[_qp];

  // Turbulent dissipative sink part
  Real dissipative_part = _test[_i][_qp] * _rho * _epsilon[_qp];

  // Source term from velocity gradients
  Real strain_tensor_double_dot_product = 0.;
  for (int m = 0; m < 3; m++)
  {
    for (int n = 0; n < 3; n++)
    {
      Real Em;
      switch (m)
      {
        case 0:
          Em = _grad_u_vel[_qp](n);
          break;
        case 1:
          Em = _grad_v_vel[_qp](n);
          break;
        case 2:
          Em = _grad_w_vel[_qp](n);
          break;
        default:
          mooseError("Unrecognized index");
      }
      Real En;
      switch (n)
      {
        case 0:
          En = _grad_u_vel[_qp](m);
          break;
        case 1:
          En = _grad_v_vel[_qp](m);
          break;
        case 2:
          En = _grad_w_vel[_qp](m);
          break;
        default:
          mooseError("Unrecognized index");
      }
      Real Emn = 0.5 * (Em + En);
      strain_tensor_double_dot_product += Emn * Emn;
    }
  }

  Real source_part = -_test[_i][_qp] * 2. * eddy_visc * strain_tensor_double_dot_product;

  return convective_part + diffusive_part + dissipative_part + source_part;
}

Real
INSK::computeQpJacobian()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  // Convective part
  Real convective_part = _rho * U * _grad_phi[_j][_qp] * _test[_i][_qp];

  // The diffusive part
  Real eddy_visc = _rho * _Cmu * std::pow(_u[_qp], 2) / _epsilon[_qp];
  Real d_eddy_visc_d_k = _rho * _Cmu * 2. * _u[_qp] / _epsilon[_qp] * _phi[_j][_qp];
  Real diffusive_part = _grad_test[_i][_qp] * ((_mu + eddy_visc / _sigk) * _grad_phi[_j][_qp] +
                                               _grad_u[_qp] * d_eddy_visc_d_k / _sigk);

  // Source term from velocity gradients
  Real strain_tensor_double_dot_product = 0.;
  for (int m = 0; m < 3; m++)
  {
    for (int n = 0; n < 3; n++)
    {
      Real Em;
      switch (m)
      {
        case 0:
          Em = _grad_u_vel[_qp](n);
          break;
        case 1:
          Em = _grad_v_vel[_qp](n);
          break;
        case 2:
          Em = _grad_w_vel[_qp](n);
          break;
        default:
          mooseError("Unrecognized index");
      }
      Real En;
      switch (n)
      {
        case 0:
          En = _grad_u_vel[_qp](m);
          break;
        case 1:
          En = _grad_v_vel[_qp](m);
          break;
        case 2:
          En = _grad_w_vel[_qp](m);
          break;
        default:
          mooseError("Unrecognized index");
      }
      Real Emn = 0.5 * (Em + En);
      strain_tensor_double_dot_product += Emn * Emn;
    }
  }

  Real source_part = -_test[_i][_qp] * 2. * d_eddy_visc_d_k * strain_tensor_double_dot_product;

  return convective_part + diffusive_part + source_part;
}

Real
INSK::computeQpOffDiagJacobian(unsigned jvar)
{
  // In Stokes/Laplacian version, off-diag Jacobian entries wrt u,v,w are zero
  if (jvar == _u_vel_var_number)
  {
    int p = 0;
    // Convective part
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](p) * _test[_i][_qp];

    // Source term from velocity gradients
    Real d_strain_tensor_double_dot_product_d_u_vel = 0.;
    for (int m = 0; m < 3; m++)
    {
      for (int n = 0; n < 3; n++)
      {
        if (p == m || p == n)
        {
          Real Em;
          switch (m)
          {
            case 0:
              Em = _grad_u_vel[_qp](n);
              break;
            case 1:
              Em = _grad_v_vel[_qp](n);
              break;
            case 2:
              Em = _grad_w_vel[_qp](n);
              break;
            default:
              mooseError("Unrecognized index");
          }
          Real En;
          switch (n)
          {
            case 0:
              En = _grad_u_vel[_qp](m);
              break;
            case 1:
              En = _grad_v_vel[_qp](m);
              break;
            case 2:
              En = _grad_w_vel[_qp](m);
              break;
            default:
              mooseError("Unrecognized index");
          }
          Real q1 = 0.;
          Real q2 = 0.;
          if (p == m)
            q1 = _grad_phi[_j][_qp](n);
          if (p == n)
            q2 = _grad_phi[_j][_qp](m);
          d_strain_tensor_double_dot_product_d_u_vel += 0.5 * (Em + En) * (q1 + q2);
        }
      }
    }

    Real eddy_visc = _rho * _Cmu * std::pow(_u[_qp], 2) / _epsilon[_qp];
    Real source_part =
        -_test[_i][_qp] * 2. * eddy_visc * d_strain_tensor_double_dot_product_d_u_vel;

    return convective_part + source_part;
  }

  else if (jvar == _v_vel_var_number)
  {
    int p = 1;
    // Convective part
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](p) * _test[_i][_qp];

    // Source term from velocity gradients
    Real d_strain_tensor_double_dot_product_d_v_vel = 0.;
    for (int m = 0; m < 3; m++)
    {
      for (int n = 0; n < 3; n++)
      {
        if (p == m || p == n)
        {
          Real Em;
          switch (m)
          {
            case 0:
              Em = _grad_u_vel[_qp](n);
              break;
            case 1:
              Em = _grad_v_vel[_qp](n);
              break;
            case 2:
              Em = _grad_w_vel[_qp](n);
              break;
            default:
              mooseError("Unrecognized index");
          }
          Real En;
          switch (n)
          {
            case 0:
              En = _grad_u_vel[_qp](m);
              break;
            case 1:
              En = _grad_v_vel[_qp](m);
              break;
            case 2:
              En = _grad_w_vel[_qp](m);
              break;
            default:
              mooseError("Unrecognized index");
          }
          Real q1 = 0.;
          Real q2 = 0.;
          if (p == m)
            q1 = _grad_phi[_j][_qp](n);
          if (p == n)
            q2 = _grad_phi[_j][_qp](m);
          d_strain_tensor_double_dot_product_d_v_vel += 0.5 * (Em + En) * (q1 + q2);
        }
      }
    }

    Real eddy_visc = _rho * _Cmu * std::pow(_u[_qp], 2) / _epsilon[_qp];
    Real source_part =
        -_test[_i][_qp] * 2. * eddy_visc * d_strain_tensor_double_dot_product_d_v_vel;

    return convective_part + source_part;
  }

  else if (jvar == _w_vel_var_number)
  {
    int p = 2;
    // Convective part
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](p) * _test[_i][_qp];

    // Source term from velocity gradients
    Real d_strain_tensor_double_dot_product_d_w_vel = 0.;
    for (int m = 0; m < 3; m++)
    {
      for (int n = 0; n < 3; n++)
      {
        if (p == m || p == n)
        {
          Real Em;
          switch (m)
          {
            case 0:
              Em = _grad_u_vel[_qp](n);
              break;
            case 1:
              Em = _grad_v_vel[_qp](n);
              break;
            case 2:
              Em = _grad_w_vel[_qp](n);
              break;
            default:
              mooseError("Unrecognized index");
          }
          Real En;
          switch (n)
          {
            case 0:
              En = _grad_u_vel[_qp](m);
              break;
            case 1:
              En = _grad_v_vel[_qp](m);
              break;
            case 2:
              En = _grad_w_vel[_qp](m);
              break;
            default:
              mooseError("Unrecognized index");
          }
          Real q1 = 0.;
          Real q2 = 0.;
          if (p == m)
            q1 = _grad_phi[_j][_qp](n);
          if (p == n)
            q2 = _grad_phi[_j][_qp](m);
          d_strain_tensor_double_dot_product_d_w_vel += 0.5 * (Em + En) * (q1 + q2);
        }
      }
    }

    Real eddy_visc = _rho * _Cmu * std::pow(_u[_qp], 2) / _epsilon[_qp];
    Real source_part =
        -_test[_i][_qp] * 2. * eddy_visc * d_strain_tensor_double_dot_product_d_w_vel;

    return convective_part + source_part;
  }

  else if (jvar == _epsilon_var_number)
  {
    // The diffusive part
    Real d_eddy_visc_d_epsilon =
        -_rho * _Cmu * std::pow(_u[_qp], 2) / std::pow(_epsilon[_qp], 2) * _phi[_j][_qp];
    Real diffusive_part = _grad_test[_i][_qp] * d_eddy_visc_d_epsilon / _sigk * _grad_u[_qp];

    // Turbulent dissipative sink part
    Real dissipative_part = _test[_i][_qp] * _rho * _phi[_j][_qp];

    // Source term from velocity gradients
    Real strain_tensor_double_dot_product = 0.;
    for (int m = 0; m < 3; m++)
    {
      for (int n = 0; n < 3; n++)
      {
        Real Em;
        switch (m)
        {
          case 0:
            Em = _grad_u_vel[_qp](n);
            break;
          case 1:
            Em = _grad_v_vel[_qp](n);
            break;
          case 2:
            Em = _grad_w_vel[_qp](n);
            break;
          default:
            mooseError("Unrecognized index");
        }
        Real En;
        switch (n)
        {
          case 0:
            En = _grad_u_vel[_qp](m);
            break;
          case 1:
            En = _grad_v_vel[_qp](m);
            break;
          case 2:
            En = _grad_w_vel[_qp](m);
            break;
          default:
            mooseError("Unrecognized index");
        }
        Real Emn = 0.5 * (Em + En);
        strain_tensor_double_dot_product += Emn * Emn;
      }
    }

    Real source_part =
        -_test[_i][_qp] * 2. * d_eddy_visc_d_epsilon * strain_tensor_double_dot_product;

    return diffusive_part + dissipative_part + source_part;
  }

  else
    return 0;
}
