#pragma once

#include "InputParameters.h"
#include "NonlinearSystemBase.h"
#include "FEProblemBase.h"
#include "MaterialProperty.h"
#include "MooseArray.h"
#include "SATauMaterial.h"

#include "libmesh/elem.h"

using MetaPhysicL::raw_value;

template <typename T>
class CDTauMaterialTempl : public T
{
public:
  static InputParameters validParams();

  CDTauMaterialTempl(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  void computeHMin();
  virtual void computeProperties() override;

  ADMaterialProperty<Real> & _tau_visc_cd;

  ADMaterialProperty<RealVectorValue> & _tau_cd;
  ADMaterialProperty<RealVectorValue> & _grad_uvel;
  ADMaterialProperty<RealVectorValue> & _grad_vvel;
  ADMaterialProperty<RealVectorValue> & _grad_wvel;

  ADReal _hmin;

  using T::_alpha;
  using T::_dt;
  using T::_has_transient;
  using T::_mu;
  using T::_qp;
  using T::_rho;
  using T::_velocity;
  using T::_grad_velocity;
  using T::_mu_tilde;
  using T::_grad_mu;
  using T::_cv1;
  using T::_sigma;

  using T::_current_elem;
  using T::_displacements;
  using T::_fe_problem;
};

typedef CDTauMaterialTempl<SATauMaterial> CDTauMaterial;

template <typename T>
InputParameters
CDTauMaterialTempl<T>::validParams()
{
  InputParameters params = T::validParams();
  params.addClassDescription(
      "This is the material class used to comptue the crosswind diffusion stabilization parameter "
      "tau_cd and tau_visc_cd  for the INS equations and "
      "the Spalart-Allmaras turbulent viscosity equation.");
  return params;
}

template <typename T>
CDTauMaterialTempl<T>::CDTauMaterialTempl(const InputParameters & parameters)
  : T(parameters),
    _tau_visc_cd(this->template declareADProperty<Real>("tau_visc_cd")),
    _tau_cd(this->template declareADProperty<RealVectorValue>("tau_cd")),
    _grad_uvel(this->template declareADProperty<RealVectorValue>("grad_uvel")),
    _grad_vvel(this->template declareADProperty<RealVectorValue>("grad_vvel")),
    _grad_wvel(this->template declareADProperty<RealVectorValue>("grad_wvel"))
{
}

template <typename T>
void
CDTauMaterialTempl<T>::computeQpProperties()
{
  T::computeQpProperties();

  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  ADReal fv1 = std::pow(chi, 3) / (std::pow(chi, 3) + std::pow(_cv1, 3));
  const auto nu = (_mu[_qp] + _mu_tilde[_qp] * fv1) / _rho[_qp];
  const auto nu_visc = (_mu[_qp] + _mu_tilde[_qp]) / _rho[_qp] / _sigma;
  const auto transient_part = _has_transient ? 4. / (_dt * _dt) : 0;

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
CDTauMaterialTempl<T>::computeHMin()
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
CDTauMaterialTempl<T>::computeProperties()
{
  computeHMin();

  T::computeProperties();
}
