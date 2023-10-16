#include "INSADMomentumTurbulentViscous.h"
#include "Assembly.h"
#include "SystemBase.h"
#include "INSADObjectTracker.h"

#include "metaphysicl/raw_type.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", INSADMomentumTurbulentViscous);

InputParameters
INSADMomentumTurbulentViscous::validParams()
{
  InputParameters params = INSADMomentumViscous::validParams();
  params.addClassDescription("Adds the turbulent viscous term to the INS momentum equation");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  return params;
}

INSADMomentumTurbulentViscous::INSADMomentumTurbulentViscous(const InputParameters & parameters)
  : INSADMomentumViscous(parameters),
    _mu_tilde(adCoupledValue("mu_tilde"))
{
}

ADRealTensorValue
INSADMomentumTurbulentViscous::qpViscousTerm()
{
  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  Real cv1 = 7.1;
  ADReal fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));

  if (_form == "laplace")
    return _mu_tilde[_qp] * fv1 * _grad_u[_qp];
  else
    return _mu_tilde[_qp] * fv1 * (_grad_u[_qp] + _grad_u[_qp].transpose());
}

ADRealVectorValue
INSADMomentumTurbulentViscous::qpAdditionalRZTerm()
{
  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  Real cv1 = 7.1;
  ADReal fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));

  // Add the u_r / r^2 term. There is an extra factor of 2 for the traction form
  ADReal resid = _mu_tilde[_qp] * fv1 * _u[_qp](_rz_radial_coord);
  if (_form == "traction")
    resid *= 2.;

  if (_use_displaced_mesh)
    return resid / (_ad_q_point[_qp](_rz_radial_coord) * _ad_q_point[_qp](_rz_radial_coord));
  else
    return resid / (_q_point[_qp](_rz_radial_coord) * _q_point[_qp](_rz_radial_coord));
}

ADReal
INSADMomentumTurbulentViscous::computeQpResidual()
{
  mooseError("computeQpResidual is not used in the INSADMomentumTurbulentViscous class");
}
