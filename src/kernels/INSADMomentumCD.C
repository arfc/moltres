#include "INSADMomentumCD.h"

registerMooseObject("MoltresApp", INSADMomentumCD);

InputParameters
INSADMomentumCD::validParams()
{
  InputParameters params = ADVectorKernelStabilized::validParams();
  params.addParam<MaterialPropertyName>(
      "tau_name", "tau_cd", "The name of the crosswind diffusion stabilization parameter.");
  params.addRequiredCoupledVar("velocity", "The velocity variable.");
  return params;
}

INSADMomentumCD::INSADMomentumCD(const InputParameters & parameters)
  : ADVectorKernelStabilized(parameters),
    _tau_cd(getADMaterialProperty<RealVectorValue>("tau_name")),
    _velocity(adCoupledVectorValue("velocity")),
    _grad_velocity(adCoupledVectorGradient("velocity")),
    _momentum_strong_residual(getADMaterialProperty<RealVectorValue>("momentum_strong_residual"))
{
}

void
INSADMomentumCD::computeResidual()
{
  prepareVectorTag(_assembly, _var.number());

  precalculateResidual();
  const unsigned int n_test = _grad_test.size();

  ADRealTensorValue vel_tensor;
  ADRealVectorValue vel_vector;
  ADRealVectorValue grad_vel;
  ADRealVectorValue stabilization;

  if (_use_displaced_mesh)
  {
    ADRealVectorValue grad_test;
    ADRealVectorValue value;
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
      for (unsigned int i = 0; i < 3; i++)
        value(i) = precomputeQpStrongResidual()(i) * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < n_test; _i++) // target for auto vectorization
      {
        for (unsigned int i = 0; i < 3; i++)
        {
          grad_test = (_grad_test[_i][_qp](i, 0),
                       _grad_test[_i][_qp](i, 1),
                       _grad_test[_i][_qp](i, 2));
          grad_vel = (_grad_velocity[_qp](i, 0),
                      _grad_velocity[_qp](i, 1),
                      _grad_velocity[_qp](i, 2));
          vel_vector = 0.;
          for (unsigned int j = 0; j < 3; j++)
            for (unsigned int k = 0; k < 3; k++)
              vel_vector(j) += grad_test(k) * vel_tensor(j, k);
          stabilization(i) = _tau_cd[_qp](i) * vel_vector * grad_vel;
        }
        _local_re(_i) += raw_value(stabilization * value);
      }
    }
  }
  else
  {
    RealVectorValue regular_grad_test;
    ADRealVectorValue value;
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
      for (unsigned int i = 0; i < 3; i++)
        value(i) = precomputeQpStrongResidual()(i) * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < n_test; _i++) // target for auto vectorization
      {
        for (unsigned int i = 0; i < 3; i++)
        {
          regular_grad_test = (_regular_grad_test[_i][_qp](i, 0),
                               _regular_grad_test[_i][_qp](i, 1),
                               _regular_grad_test[_i][_qp](i, 2));
          grad_vel = (_grad_velocity[_qp](i, 0),
                      _grad_velocity[_qp](i, 1),
                      _grad_velocity[_qp](i, 2));
          vel_vector = 0.;
          for (unsigned int j = 0; j < 3; j++)
            for (unsigned int k = 0; k < 3; k++)
              vel_vector(j) += regular_grad_test(k) * vel_tensor(j, k);
          stabilization(i) = _tau_cd[_qp](i) * vel_vector * grad_vel;
        }
        _local_re(_i) += raw_value(stabilization * value);
      }
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
INSADMomentumCD::computeResidualsForJacobian()
{
  if (_residuals.size() != _grad_test.size())
    _residuals.resize(_grad_test.size(), 0);
  for (auto & r : _residuals)
    r = 0;

  precalculateResidual();

  ADRealTensorValue vel_tensor;
  ADRealVectorValue vel_vector;
  ADRealVectorValue grad_vel;
  ADRealVectorValue stabilization;

  if (_use_displaced_mesh)
  {
    ADRealVectorValue grad_test;
    ADRealVectorValue value;
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
      for (unsigned int i = 0; i < 3; i++)
        value(i) = precomputeQpStrongResidual()(i) * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i++)
      {
        for (unsigned int i = 0; i < 3; i++)
        {
          grad_test = (_grad_test[_i][_qp](i, 0),
                       _grad_test[_i][_qp](i, 1),
                       _grad_test[_i][_qp](i, 2));
          grad_vel = (_grad_velocity[_qp](i, 0),
                      _grad_velocity[_qp](i, 1),
                      _grad_velocity[_qp](i, 2));
          vel_vector = 0.;
          for (unsigned int j = 0; j < 3; j++)
            for (unsigned int k = 0; k < 3; k++)
              vel_vector(j) += grad_test(k) * vel_tensor(j, k);
          stabilization(i) = _tau_cd[_qp](i) * vel_vector * grad_vel;
        }
        _residuals[_i] += stabilization * value;
      }
    }
  }
  else
  {
    RealVectorValue regular_grad_test;
    ADRealVectorValue value;
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
      for (unsigned int i = 0; i < 3; i++)
        value(i) = precomputeQpStrongResidual()(i) * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i++)
      {
        for (unsigned int i = 0; i < 3; i++)
        {
          regular_grad_test = (_regular_grad_test[_i][_qp](i, 0),
                               _regular_grad_test[_i][_qp](i, 1),
                               _regular_grad_test[_i][_qp](i, 2));
          grad_vel = (_grad_velocity[_qp](i, 0),
                      _grad_velocity[_qp](i, 1),
                      _grad_velocity[_qp](i, 2));
          vel_vector = 0.;
          for (unsigned int j = 0; j < 3; j++)
            for (unsigned int k = 0; k < 3; k++)
              vel_vector(j) += regular_grad_test(k) * vel_tensor(j, k);
          stabilization(i) = _tau_cd[_qp](i) * vel_vector * grad_vel;
        }
        _residuals[_i] += stabilization * value;
      }
    }
  }
}

ADRealVectorValue
INSADMomentumCD::precomputeQpStrongResidual()
{
  return _momentum_strong_residual[_qp];
}

ADRealVectorValue
INSADMomentumCD::computeQpStabilization()
{
  ADRealVectorValue zero_vector(0., 0., 0.);
  return zero_vector;
}
