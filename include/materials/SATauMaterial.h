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
  const ADVariableValue * _visc_dot;
  ADMaterialProperty<Real> & _tau_visc;
  const VariableValue & _wall_dist;
  const bool _use_ft2_term;

  ADMaterialProperty<Real> & _strain_mag;
  ADMaterialProperty<Real> & _convection_strong_residual;
  ADMaterialProperty<Real> & _destruction_strong_residual;
  ADMaterialProperty<Real> & _diffusion_strong_residual;
  ADMaterialProperty<Real> & _source_strong_residual;
  ADMaterialProperty<Real> & _time_strong_residual;
  ADMaterialProperty<Real> & _visc_strong_residual;

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
};

typedef SATauMaterialTempl<INSADTauMaterial> SATauMaterial;
