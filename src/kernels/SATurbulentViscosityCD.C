#include "SATurbulentViscosityCD.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", SATurbulentViscosityCD);

InputParameters
SATurbulentViscosityCD::validParams()
{
  InputParameters params = ADKernelStabilized::validParams();
  params.addClassDescription("Adds the crosswind diffusion stabilization term in the turbulent "
                             "viscosity equation from the Spalart-Allmaras model");
  params.addParam<MaterialPropertyName>(
      "tau_name",
      "tau_visc_cd",
      "The name of the crosswind stabilization parameter tau for turbulent viscosity");
  params.addRequiredCoupledVar("velocity", "The velocity variable.");
  return params;
}

SATurbulentViscosityCD::SATurbulentViscosityCD(const InputParameters & parameters)
  : ADKernelStabilized(parameters),
    _tau_visc_cd(getADMaterialProperty<Real>("tau_name")),
    _velocity(adCoupledVectorValue("velocity")),
    _visc_strong_residual(getADMaterialProperty<Real>("visc_strong_residual"))
{
}

ADRealVectorValue
SATurbulentViscosityCD::computeQpStabilization()
{
  return _tau_visc_cd[_qp] * _grad_u[_qp];
}

ADReal
SATurbulentViscosityCD::precomputeQpStrongResidual()
{
  return std::abs(_visc_strong_residual[_qp]);
}

void
SATurbulentViscosityCD::computeResidual()
{
  prepareVectorTag(_assembly, _var.number());

  precalculateResidual();
  const unsigned int n_test = _grad_test.size();

  ADRealTensorValue vel_tensor;

  ADRealVectorValue vel_vector;
  if (_use_displaced_mesh)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      ADReal vel_mag = _velocity[_qp] * _velocity[_qp];
      vel_tensor = 0.;
      for (unsigned int i = 0; i < 3; i++)
        vel_tensor(i, i) = 1.;
      if (raw_value(vel_mag) > 0.)
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_tensor(i, j) -= _velocity[_qp](i) * _velocity[_qp](j) / vel_mag;
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < n_test; _i++) // target for auto vectorization
      {
        vel_vector = 0.;
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_vector(i) += _grad_test[_i][_qp](j) * vel_tensor(i, j);
        _local_re(_i) += raw_value(vel_vector * computeQpStabilization() * value);
      }
    }
  else
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      ADReal vel_mag = _velocity[_qp] * _velocity[_qp];
      vel_tensor = 0.;
      for (unsigned int i = 0; i < 3; i++)
        vel_tensor(i, i) = 1.;
      if (raw_value(vel_mag) > 0.)
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_tensor(i, j) -= _velocity[_qp](i) * _velocity[_qp](j) / vel_mag;
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < n_test; _i++) // target for auto vectorization
      {
        vel_vector = 0.;
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_vector(i) += _regular_grad_test[_i][_qp](j) * vel_tensor(i, j);
        _local_re(_i) += raw_value(vel_vector * computeQpStabilization() * value);
      }
    }

  accumulateTaggedLocalResidual();

  if (_has_save_in)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (unsigned int i = 0; i < _save_in.size(); i++)
      _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
  }
}

void
SATurbulentViscosityCD::computeResidualsForJacobian()
{
  if (_residuals.size() != _grad_test.size())
    _residuals.resize(_grad_test.size(), 0);
  for (auto & r : _residuals)
    r = 0;

  precalculateResidual();

  ADRealTensorValue vel_tensor;

  ADRealVectorValue vel_vector(0., 0., 0.);
  if (_use_displaced_mesh)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      ADReal vel_mag = _velocity[_qp] * _velocity[_qp];
      vel_tensor = 0.;
      for (unsigned int i = 0; i < 3; i++)
        vel_tensor(i, i) = 1.;
      if (raw_value(vel_mag) > 0.)
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_tensor(i, j) -= _velocity[_qp](i) * _velocity[_qp](j) / vel_mag;
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i++)
      {
        vel_vector = 0.;
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_vector(i) += _grad_test[_i][_qp](j) * vel_tensor(i, j);
        _residuals[_i] += (vel_vector * computeQpStabilization() * value);
      }

    }
  else
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      ADReal vel_mag = _velocity[_qp] * _velocity[_qp];
      vel_tensor = 0.;
      for (unsigned int i = 0; i < 3; i++)
        vel_tensor(i, i) = 1.;
      if (raw_value(vel_mag) > 0.)
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_tensor(i, j) -= _velocity[_qp](i) * _velocity[_qp](j) / vel_mag;
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i++)
      {
        vel_vector = 0.;
        for (unsigned int i = 0; i < 3; i++)
          for (unsigned int j = 0; j < 3; j++)
            vel_vector(i) += _regular_grad_test[_i][_qp](j) * vel_tensor(i, j);
        _residuals[_i] += (vel_vector * computeQpStabilization() * value);
      }
    }
}
