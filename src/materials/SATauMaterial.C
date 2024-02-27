#include "SATauMaterial.h"
#include "INSADTauMaterial.h"

registerMooseObject("MoltresApp", SATauMaterial);

template <typename T>
InputParameters
SATauMaterialTempl<T>::validParams()
{
  InputParameters params = T::validParams();
  params.addClassDescription(
      "This is the material class used to compute the stabilization parameter tau_viscosity "
      "for the Spalart-Allmaras turbulent viscosity equation.");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  params.addRequiredCoupledVar("wall_distance_var", "Wall distance aux variable name");
  params.addParam<bool>("use_ft2_term", false, "Whether to apply the f_t2 term in the equation");
  return params;
}

template <typename T>
SATauMaterialTempl<T>::SATauMaterialTempl(const InputParameters & parameters)
  : T(parameters),
    _sigma(2.0/3.0),
    _cb1(0.1355),
    _cb2(0.622),
    _kappa(0.41),
    _cw1(_cb1 / _kappa / _kappa + (1 + _cb2) / _sigma),
    _cw2(0.3),
    _cw3(2.0),
    _cv1(7.1),
    _ct1(1.0),
    _ct2(2.0),
    _ct3(1.2),
    _ct4(0.5),
    _mu_tilde(this->template adCoupledValue("mu_tilde")),
    _grad_mu(this->template adCoupledGradient("mu_tilde")),
    _tau_visc(this->template declareADProperty<Real>("tau_viscosity")),
    _wall_dist(this->template coupledValue("wall_distance_var")),
    _use_ft2_term(this->template getParam<bool>("use_ft2_term")),
    _strain_mag(this->template declareADProperty<Real>("strain_mag")),
    _convection_strong_residual(this->template
        declareADProperty<Real>("convection_strong_residual")),
    _destruction_strong_residual(this->template
        declareADProperty<Real>("destruction_strong_residual")),
    _diffusion_strong_residual(this->template
        declareADProperty<Real>("diffusion_strong_residual")),
    _source_strong_residual(this->template
        declareADProperty<Real>("source_strong_residual")),
    _time_strong_residual(this->template
        declareADProperty<Real>("time_strong_residual")),
    _visc_strong_residual(this->template
        declareADProperty<Real>("visc_strong_residual"))
{
}

template <typename T>
void
SATauMaterialTempl<T>::subdomainSetup()
{
  T::subdomainSetup();

  if (_has_transient)
    _visc_dot = & this->template adCoupledDot("mu_tilde");
  else
    _visc_dot = nullptr;
}

template <typename T>
void
SATauMaterialTempl<T>::computeQpProperties()
{
  T::computeQpProperties();

  // Compute strain rate and vorticity magnitudes
  _strain_mag[_qp] = 2.0 * Utility::pow<2>(_grad_velocity[_qp](0, 0)) +
                     2.0 * Utility::pow<2>(_grad_velocity[_qp](1, 1)) +
                     2.0 * Utility::pow<2>(_grad_velocity[_qp](2, 2)) +
                     Utility::pow<2>(_grad_velocity[_qp](0, 2) + _grad_velocity[_qp](2, 0)) +
                     Utility::pow<2>(_grad_velocity[_qp](0, 1) + _grad_velocity[_qp](1, 0)) +
                     Utility::pow<2>(_grad_velocity[_qp](1, 2) + _grad_velocity[_qp](2, 1));
  _strain_mag[_qp] = std::sqrt(_strain_mag[_qp] + 1e-16);
  ADReal vorticity_mag =
      Utility::pow<2>(_grad_velocity[_qp](0, 2) - _grad_velocity[_qp](2, 0)) +
      Utility::pow<2>(_grad_velocity[_qp](0, 1) - _grad_velocity[_qp](1, 0)) +
      Utility::pow<2>(_grad_velocity[_qp](1, 2) - _grad_velocity[_qp](2, 1));
  vorticity_mag = std::sqrt(vorticity_mag + 1e-16);

  // Compute relevant parameters for the SA equation
  const Real d = std::max(_wall_dist[_qp], 1e-16); // Avoid potential division by zero
  const ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  const ADReal fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(_cv1));
  const ADReal fv2 = 1.0 - chi / (1. + chi * fv1);
  const ADReal S_tilde =
    vorticity_mag + _mu_tilde[_qp] * fv2 / (_kappa * _kappa * d * d * _rho[_qp]);
  const ADReal S = S_tilde + 2 * std::min(0.0, _strain_mag[_qp] - vorticity_mag);
  ADReal r;
  if (S_tilde <= 0.0) // Avoid potential division by zero
    r = 10.;
  else
    r = std::min(_mu_tilde[_qp] / (S_tilde * _kappa * _kappa * d * d * _rho[_qp]), 10.0);
  const ADReal g = r + _cw2 * (Utility::pow<6>(r) - r);
  const ADReal fw = g * std::pow((1. + Utility::pow<6>(_cw3)) /
                           (Utility::pow<6>(g) + Utility::pow<6>(_cw3)),
                           1.0 / 6.0);

  // Compute strong forms of the SA equation
  if (_use_ft2_term) // Whether to apply the f_t2 term in the SA equation
  {
    const ADReal ft2 = _ct3 * std::exp(-_ct4 * chi * chi);
    _destruction_strong_residual[_qp] =
      (_cw1 * fw - _cb1 * ft2 / _kappa / _kappa) * Utility::pow<2>(_mu_tilde[_qp] / d);
    _source_strong_residual[_qp] = -(1 - ft2) * _rho[_qp] * _cb1 * S * _mu_tilde[_qp];
  }
  else
  {
    _destruction_strong_residual[_qp] = _cw1 * fw * Utility::pow<2>(_mu_tilde[_qp] / d);
    _source_strong_residual[_qp] = -_rho[_qp] * _cb1 * S * _mu_tilde[_qp];
  }
  _convection_strong_residual[_qp] = _rho[_qp] * _velocity[_qp] * _grad_mu[_qp];
  _diffusion_strong_residual[_qp] = -1.0 / _sigma * _cb2 * (_grad_mu[_qp] * _grad_mu[_qp]);
  if (_has_transient)
    _time_strong_residual[_qp] = (*_visc_dot)[_qp] * _rho[_qp];
  _visc_strong_residual[_qp] = _has_transient ? _time_strong_residual[_qp] : 0.0;
  _visc_strong_residual[_qp] += (_convection_strong_residual[_qp] +
                                 _destruction_strong_residual[_qp] +
                                 _diffusion_strong_residual[_qp] +
                                 _source_strong_residual[_qp]);

  // Compute the tau stabilization parameter for mu_tilde SUPG stabilization
  const auto nu_visc = (_mu[_qp] + _mu_tilde[_qp]) / _rho[_qp] / _sigma;
  const auto transient_part = _has_transient ? 4.0 / (_dt * _dt) : 0.0;
  const auto speed = NS::computeSpeed(_velocity[_qp]);
  _tau_visc[_qp] = _alpha / std::sqrt(transient_part +
                                      (2.0 * speed / _hmax) * (2.0 * speed / _hmax) +
                                      9.0 * (4.0 * nu_visc / (_hmax * _hmax)) *
                                      4.0 * (nu_visc / (_hmax * _hmax)));

  // Replace the nu value in the tau stabilization parameter for INS SUPG stabilization
  const auto nu = (_mu[_qp] + _mu_tilde[_qp] * fv1) / _rho[_qp];
  _tau[_qp] = _alpha / std::sqrt(transient_part +
                                 (2.0 * speed / _hmax) * (2.0 * speed / _hmax) +
                                 9.0 * (4.0 * nu / (_hmax * _hmax)) *
                                 4.0 * (nu / (_hmax * _hmax)));
}

template class SATauMaterialTempl<INSADTauMaterial>;
