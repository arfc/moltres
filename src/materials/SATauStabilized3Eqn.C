#include "SATauStabilized3Eqn.h"

registerMooseObject("MoltresApp", SATauStabilized3Eqn);

InputParameters
SATauStabilized3Eqn::validParams()
{
  InputParameters params = SATauMaterialTempl<INSADStabilized3Eqn>::validParams();
  params.addClassDescription("This is the material class used to add the turbulent diffusion "
                             "strong residual contribution for the stabilization of the "
                             "energy equation.");
  params.addParam<Real>("prandtl_number", 0.85, "Turbulent Prandtl number");
  return params;
}

SATauStabilized3Eqn::SATauStabilized3Eqn(const InputParameters & parameters)
  : SATauMaterialTempl<INSADStabilized3Eqn>(parameters), _pr(getParam<Real>("prandtl_number"))
{
}

void
SATauStabilized3Eqn::computeQpProperties()
{
  using std::sqrt;

  SATauMaterialTempl<INSADStabilized3Eqn>::computeQpProperties();

  const ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  const Real cv1 = 7.1;
  const ADReal fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));

  const auto dissipation_coefficient =
      _k[_qp] / (_rho[_qp] * _cp[_qp]) + _mu_tilde[_qp] * fv1 / (_pr * _rho[_qp]);
  const auto transient_part = _has_energy_transient ? 4.0 / (_dt * _dt) : 0.0;
  const auto speed = NS::computeSpeed(_velocity[_qp]);
  _tau_energy[_qp] = _alpha / sqrt(transient_part + (2.0 * speed / _hmax) * (2.0 * speed / _hmax) +
                                   9.0 * (4.0 * dissipation_coefficient / (_hmax * _hmax)) *
                                       (4.0 * dissipation_coefficient / (_hmax * _hmax)));

  // Turbulent diffusion
  _temperature_strong_residual[_qp] -=
      _cp[_qp] * fv1 / _pr *
      (_mu_tilde[_qp] * _second_temperature[_qp].tr() + _grad_mu[_qp] * _grad_temperature[_qp]);
}
