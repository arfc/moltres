#include "ScalarAdvectionArtDiff.h"
#include "Assembly.h"

registerMooseObject("MoltresApp", ScalarAdvectionArtDiff);

InputParameters
ScalarAdvectionArtDiff::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addClassDescription("This class computes the residual and Jacobian "
      "contributions from the artificial diffusion term, "
      "$D' = \\tau |u| \\Delta x / 2$.");
  params.addParam<Real>("scale", 1., "Amount to scale artificial diffusion.");

  // Coupled variables
  params.addCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", "y-velocity");
  params.addCoupledVar("w", "z-velocity");
  params.addParam<Real>(
      "u_def", 0, "Allows user to specify constant value for u component of velocity.");
  params.addParam<Real>(
      "v_def", 0, "Allows user to specify constant value for v component of velocity.");
  params.addParam<Real>(
      "w_def", 0, "Allows user to specify constant value for w component of velocity.");
  params.addParam<MaterialPropertyName>(
      "diffusivity", "D", "The diffusivity value or material property");
  params.addParam<Real>(
      "conc_scaling", 1, "The amount by which to scale the concentration variable.");
  return params;
}

ScalarAdvectionArtDiff::ScalarAdvectionArtDiff(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),

    _scale(getParam<Real>("scale")),
    // Coupled variables
    _u_vel(isCoupled("u") ? coupledValue("u") : _u_def),
    _v_vel(isCoupled("v") ? coupledValue("v") : _v_def),
    _w_vel(isCoupled("w") ? coupledValue("w") : _w_def),

    // Variable numberings
    _u_vel_var_number(coupled("u")),
    _v_vel_var_number(coupled("v")),
    _w_vel_var_number(coupled("w")),
    _D(getMaterialProperty<Real>("diffusivity")),
    _conc_scaling(getParam<Real>("conc_scaling")),
    _current_elem_volume(_assembly.elemVolume())
{
  if (!(isCoupled("u")))
    _u_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("u_def")));
  if (!(isCoupled("v")))
    _v_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("v_def")));
  if (!(isCoupled("w")))
    _w_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("w_def")));
}

Real
ScalarAdvectionArtDiff::tau()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
  Real u_norm = U.norm();
  Real h;
  mooseAssert(_mesh.dimension() >= 1 && _mesh.dimension() <= 3, "The dimension should always be between 1 and 3");
  if (_mesh.dimension() == 1)
    h = _current_elem->volume();
  else if (_mesh.dimension() == 2)
    h = std::sqrt(_current_elem->volume());
  else
    h = std::cbrt(_current_elem->volume());
  Real gamma = u_norm * h / 2. / _D[_qp];
  if (gamma <= 1e-10)
    return 0.; // low-Peclet flow => no artificial diffusion
  else if (gamma >= 5.)
    return 1. - 1. / gamma; // prevent overflow

  return 1. / std::tanh(gamma) - 1. / gamma;
}

Real
ScalarAdvectionArtDiff::computeQpResidual()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
  mooseAssert(_mesh.dimension() >= 1 && _mesh.dimension() <= 3, "The dimension should always be between 1 and 3");
  Real h;
  if (_mesh.dimension() == 1)
    h = _current_elem->volume();
  else if (_mesh.dimension() == 2)
    h = std::sqrt(_current_elem->volume());
  else
    h = std::cbrt(_current_elem->volume());
  Real delta = U.norm() * h / 2. * ScalarAdvectionArtDiff::tau();

  return -_grad_test[_i][_qp] * -delta * computeConcentrationGradient(_u, _grad_u, _qp) * _scale *
         _conc_scaling;
}

Real
ScalarAdvectionArtDiff::computeQpJacobian()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
  mooseAssert(_mesh.dimension() >= 1 && _mesh.dimension() <= 3, "The dimension should always be between 1 and 3");
  Real h;
  if (_mesh.dimension() == 1)
    h = _current_elem->volume();
  else if (_mesh.dimension() == 2)
    h = std::sqrt(_current_elem->volume());
  else
    h = std::cbrt(_current_elem->volume());
  Real delta = U.norm() * h / 2. * ScalarAdvectionArtDiff::tau();

  return -_grad_test[_i][_qp] * -delta *
         computeConcentrationGradientDerivative(_u, _grad_u, _phi, _grad_phi, _j, _qp) * _scale *
         _conc_scaling;
}

Real
ScalarAdvectionArtDiff::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real h;
  if (_mesh.dimension() == 1)
    h = _current_elem->volume();
  else if (_mesh.dimension() == 2)
    h = std::sqrt(_current_elem->volume());
  else if (_mesh.dimension() == 3)
    h = std::cbrt(_current_elem->volume());
  if (jvar == _u_vel_var_number)
  {
    RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
    Real d_delta_d_u_vel =
        _u_vel[_qp] * _phi[_j][_qp] / U.norm() *
        h / 2. * ScalarAdvectionArtDiff::tau();
    return -_grad_test[_i][_qp] * -d_delta_d_u_vel *
           computeConcentrationGradient(_u, _grad_u, _qp) * _scale * _conc_scaling;
  }

  else if (jvar == _v_vel_var_number)
  {
    RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
    Real d_delta_d_v_vel =
        _v_vel[_qp] * _phi[_j][_qp] / U.norm() *
        h / 2. * ScalarAdvectionArtDiff::tau();
    return -_grad_test[_i][_qp] * -d_delta_d_v_vel *
           computeConcentrationGradient(_u, _grad_u, _qp) * _scale * _conc_scaling;
  }

  else if (jvar == _w_vel_var_number)
  {
    RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);
    Real d_delta_d_w_vel =
        _w_vel[_qp] * _phi[_j][_qp] / U.norm() *
        h / 2. * ScalarAdvectionArtDiff::tau();
    return -_grad_test[_i][_qp] * -d_delta_d_w_vel *
           computeConcentrationGradient(_u, _grad_u, _qp) * _scale * _conc_scaling;
  }
  else
    return 0.0;
}
