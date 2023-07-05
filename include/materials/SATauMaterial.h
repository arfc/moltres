#pragma once

#include "InputParameters.h"
#include "NonlinearSystemBase.h"
#include "FEProblemBase.h"
#include "MaterialProperty.h"
#include "MooseArray.h"
#include "INSADTauMaterial.h"

#include "libmesh/elem.h"

using MetaPhysicL::raw_value;

template <typename T>
class SATauMaterialTempl : public T
{
public:
  static InputParameters validParams();

  SATauMaterialTempl(const InputParameters & parameters);

  void subdomainSetup() override;

protected:
  virtual void computeQpProperties() override;
  void computeHMin();
  virtual void computeProperties() override;

  const Real _sigma;
  const Real _cb1;
  const Real _cb2;
  const Real _kappa;
  const Real _cw1;
  const Real _cw2;
  const Real _cw3;
  const Real _cv1;
  const Real _ct1;
  const Real _ct2;
  const Real _ct3;
  const Real _ct4;
  const ADVariableValue & _mu_tilde;
  const ADVariableGradient & _grad_mu;
//  const ADVariableSecond & _second_mu;
  const ADVariableValue * _visc_dot;
  ADMaterialProperty<Real> & _tau_visc;
  ADMaterialProperty<Real> & _tau_visc_cd;
  const VariableValue & _wall_dist;
  const bool _use_ft2_term;

  ADMaterialProperty<Real> & _convection_strong_residual;
  ADMaterialProperty<Real> & _destruction_strong_residual;
  ADMaterialProperty<Real> & _diffusion_strong_residual;
  ADMaterialProperty<Real> & _source_strong_residual;
  ADMaterialProperty<Real> & _time_strong_residual;
  ADMaterialProperty<Real> & _visc_strong_residual;

  ADMaterialProperty<RealVectorValue> & _tau_cd;
  ADMaterialProperty<RealVectorValue> & _grad_uvel;
  ADMaterialProperty<RealVectorValue> & _grad_vvel;
  ADMaterialProperty<RealVectorValue> & _grad_wvel;

  ADReal _hmin;

  using T::_alpha;
  using T::_dt;
  using T::_has_transient;
  using T::_hmax;
  using T::_mu;
  using T::_qp;
  using T::_rho;
  using T::_velocity;
  using T::_grad_velocity;
  using T::_tau;

  using T::_current_elem;
  using T::_displacements;
  using T::_fe_problem;
};

typedef SATauMaterialTempl<INSADTauMaterial> SATauMaterial;

template <typename T>
InputParameters
SATauMaterialTempl<T>::validParams()
{
  InputParameters params = T::validParams();
  params.addClassDescription(
      "This is the material class used to comptue the stabilization parameter tau_viscosity "
      "for the Spalart-Allmaras turbulent viscosity equation.");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  params.addRequiredCoupledVar("wall_distance_var", "Wall distance aux variable name");
  params.addParam<bool>("use_ft2_term", false, "Whether to apply the f_t2 term in the equation");
  return params;
}

template <typename T>
SATauMaterialTempl<T>::SATauMaterialTempl(const InputParameters & parameters)
  : T(parameters),
    _sigma(2./3.),
    _cb1(0.1355),
    _cb2(0.622),
    _kappa(0.41),
    _cw1(_cb1 / _kappa / _kappa + (1 + _cb2) / _sigma),
    _cw2(0.3),
    _cw3(2.),
    _cv1(7.1),
    _ct1(1.),
    _ct2(2.),
    _ct3(1.2),
    _ct4(0.5),
    _mu_tilde(this->template adCoupledValue("mu_tilde")),
    _grad_mu(this->template adCoupledGradient("mu_tilde")),
//    _second_mu(this->template adCoupledSecond("mu_tilde")),
    _tau_visc(this->template declareADProperty<Real>("tau_viscosity")),
    _tau_visc_cd(this->template declareADProperty<Real>("tau_visc_cd")),
    _wall_dist(this->template coupledValue("wall_distance_var")),
    _use_ft2_term(this->template getParam<bool>("use_ft2_term")),
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
        declareADProperty<Real>("visc_strong_residual")),
    _tau_cd(this->template declareADProperty<RealVectorValue>("tau_cd")),
    _grad_uvel(this->template declareADProperty<RealVectorValue>("grad_uvel")),
    _grad_vvel(this->template declareADProperty<RealVectorValue>("grad_vvel")),
    _grad_wvel(this->template declareADProperty<RealVectorValue>("grad_wvel"))
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

  ADRealTensorValue vorticity = .5 * (_grad_velocity[_qp] - _grad_velocity[_qp].transpose());
  ADReal vorticity_mag = 0.;
  for (unsigned int i = 0; i < 3; i++)
    for (unsigned int j = 0; j < 3; j++)
      vorticity_mag += 2 * vorticity(i, j) * vorticity(i, j);
  vorticity_mag += 1e-16;
  
  ADRealTensorValue strain = .5 * (_grad_velocity[_qp] + _grad_velocity[_qp].transpose());
  ADReal strain_mag = 0.;
  for (unsigned int i = 0; i < 3; i++)
    for (unsigned int j = 0; j < 3; j++)
      strain_mag += 2 * strain(i, j) * strain(i, j);
  strain_mag += 1e-16;

  Real d = std::max(_wall_dist[_qp], 1e-16);
  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  ADReal fv1 = std::pow(chi, 3) / (std::pow(chi, 3) + std::pow(_cv1, 3));
  ADReal fv2 = 1. - chi / (1. + chi * fv1);
  ADReal Omega = std::sqrt(vorticity_mag);
  ADReal S_ij = std::sqrt(strain_mag);
  ADReal S_tilde = Omega + _mu_tilde[_qp] * fv2 / (_kappa * _kappa * d * d * _rho[_qp]);
  ADReal S = S_tilde + 2 * std::min(0., S_ij - Omega);
  ADReal r;
  if (S_tilde <= 0.)
    r = 10.;
  else
    r = std::min(_mu_tilde[_qp] / (S_tilde * _kappa * _kappa * d * d * _rho[_qp]), 10.);
  ADReal g = r + _cw2 * (std::pow(r, 6) - r);
  ADReal fw = g * std::pow((1. + std::pow(_cw3, 6)) / (std::pow(g, 6) + std::pow(_cw3, 6)),
                           1. / 6.);

  if (_use_ft2_term)
  {
    ADReal ft2 = _ct3 * std::exp(-_ct4 * chi * chi);
    _destruction_strong_residual[_qp] =
      (_cw1 * fw - _cb1 * ft2 / _kappa / _kappa) * std::pow(_mu_tilde[_qp] / d, 2);
    _source_strong_residual[_qp] = -(1 - ft2) * _rho[_qp] * _cb1 * S * _mu_tilde[_qp];
  }
  else
  {
    _destruction_strong_residual[_qp] = _cw1 * fw * std::pow(_mu_tilde[_qp] / d, 2);
    _source_strong_residual[_qp] = -_rho[_qp] * _cb1 * S * _mu_tilde[_qp];
  }

  _convection_strong_residual[_qp] = _rho[_qp] * _velocity[_qp] * _grad_mu[_qp];
//  _diffusion_strong_residual[_qp] = -1. / _sigma * ((_mu[_qp] + _mu_tilde[_qp]) *
//    _second_mu[_qp].tr() + (1 + _cb2) * _grad_mu[_qp] * _grad_mu[_qp]);
  _diffusion_strong_residual[_qp] = -1. / _sigma * _cb2 * (_grad_mu[_qp] * _grad_mu[_qp]);
  if (_has_transient)
    _time_strong_residual[_qp] = (*_visc_dot)[_qp] * _rho[_qp];

  _visc_strong_residual[_qp] = _has_transient ? _time_strong_residual[_qp] : 0.;
  _visc_strong_residual[_qp] += (_convection_strong_residual[_qp] +
                                 _destruction_strong_residual[_qp] +
                                 _diffusion_strong_residual[_qp] +
                                 _source_strong_residual[_qp]);

  const auto nu_visc = (_mu[_qp] + _mu_tilde[_qp]) / _rho[_qp] / _sigma;
  const auto transient_part = _has_transient ? 4. / (_dt * _dt) : 0.;
  const auto speed = NS::computeSpeed(_velocity[_qp]);
  _tau_visc[_qp] = _alpha / std::sqrt(transient_part +
                                      (2. * speed / _hmax) * (2. * speed / _hmax) +
                                      9. * (4. * nu_visc / (_hmax * _hmax)) *
                                      4. * (nu_visc / (_hmax * _hmax)));

  const auto nu = (_mu[_qp] + _mu_tilde[_qp] * fv1) / _rho[_qp];
  _tau[_qp] = _alpha / std::sqrt(transient_part +
                                 (2. * speed / _hmax) * (2. * speed / _hmax) +
                                 9. * (4. * nu / (_hmax * _hmax)) *
                                 4. * (nu / (_hmax * _hmax)));

  _grad_uvel[_qp] = (_grad_velocity[_qp](0, 0),
                     _grad_velocity[_qp](0, 1),
                     _grad_velocity[_qp](0, 2));
  _grad_vvel[_qp] = (_grad_velocity[_qp](1, 0),
                     _grad_velocity[_qp](1, 1),
                     _grad_velocity[_qp](1, 2));
  _grad_wvel[_qp] = (_grad_velocity[_qp](2, 0),
                     _grad_velocity[_qp](2, 1),
                     _grad_velocity[_qp](2, 2));

  if (std::norm(raw_value(_grad_uvel[_qp])) <= 0.)
    _tau_cd[_qp](0) = 0.;
  else
  {
    ADReal proj_speed_u = (_grad_uvel[_qp] * _velocity[_qp]) / std::norm(_grad_uvel[_qp]);
    if (proj_speed_u > 0.)
      _tau_cd[_qp](0) =
        _alpha / std::sqrt(transient_part +
                           std::pow(std::max(0., .7 - 2. * nu / _hmin / proj_speed_u) *
                                    _hmin / std::norm(_grad_uvel[_qp]) / 2, 2.));
    else
      _tau_cd[_qp](0) = 0.;
  }

  if (std::norm(raw_value(_grad_vvel[_qp])) <= 0.)
    _tau_cd[_qp](1) = 0.;
  else
  {
    ADReal proj_speed_v = (_grad_vvel[_qp] * _velocity[_qp]) / std::norm(_grad_vvel[_qp]);
    if (proj_speed_v > 0.)
      _tau_cd[_qp](1) =
        _alpha / std::sqrt(transient_part +
                           std::pow(std::max(0., .7 - 2. * nu / _hmin / proj_speed_v) *
                                    _hmin / std::norm(_grad_vvel[_qp]) / 2, 2.));
    else
      _tau_cd[_qp](1) = 0.;
  }

  if (std::norm(raw_value(_grad_wvel[_qp])) <= 0.)
    _tau_cd[_qp](2) = 0.;
  else
  {
    ADReal proj_speed_w = (_grad_wvel[_qp] * _velocity[_qp]) / std::norm(_grad_wvel[_qp]);
    if (proj_speed_w > 0.)
      _tau_cd[_qp](2) =
        _alpha / std::sqrt(transient_part +
                           std::pow(std::max(0., .7 - 2. * nu / _hmin / proj_speed_w) *
                                    _hmin / std::norm(_grad_wvel[_qp]) / 2, 2.));
    else
      _tau_cd[_qp](2) = 0.;
  }

  if ((std::norm(raw_value(_grad_mu[_qp])) <= 0.))
    _tau_visc_cd[_qp] = 0.;
  else
  {
    ADReal proj_speed_visc = (_grad_mu[_qp] * _velocity[_qp]) / std::norm(_grad_mu[_qp]);
    if (proj_speed_visc > 0.)
      _tau_visc_cd[_qp] = 
        _alpha / std::sqrt(transient_part +
                           std::pow(std::max(0., .7 - 2. * nu_visc / _hmin / proj_speed_visc) *
                                    _hmin / std::norm(_grad_mu[_qp]) / 2, 2.));
    else
      _tau_visc_cd[_qp] = 0.;
  }
}

template <typename T>
void
SATauMaterialTempl<T>::computeHMin()
{
  if (!_displacements.size())
  {
    _hmin = _current_elem->hmin();
    return;
  }

  _hmin = 0;

  for (unsigned int n_outer = 0; n_outer < _current_elem->n_vertices(); n_outer++)
    for (unsigned int n_inner = n_outer + 1; n_inner < _current_elem->n_vertices(); n_inner++)
    {
      VectorValue<DualReal> diff = (_current_elem->point(n_outer) - _current_elem->point(n_inner));
      unsigned dimension = 0;
      for (const auto & disp_num : _displacements)
      {
        diff(dimension)
            .derivatives()[disp_num * _fe_problem.getNonlinearSystemBase().getMaxVarNDofsPerElem() +
                           n_outer] = 1.;
        diff(dimension++)
            .derivatives()[disp_num * _fe_problem.getNonlinearSystemBase().getMaxVarNDofsPerElem() +
                           n_inner] = -1.;
      }

      _hmin = std::max(_hmin, diff.norm_sq());
    }

  _hmin = std::sqrt(_hmin);
}

template <typename T>
void
SATauMaterialTempl<T>::computeProperties()
{
  computeHMin();

  T::computeProperties();
}
