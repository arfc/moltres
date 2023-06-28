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
  ADRealVectorValue grad_vel;
  ADRealVectorValue stabilization;

  if (_use_displaced_mesh)
  {
    ADRealVectorValue grad_test;
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
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
          stabilization(i) = _tau_cd[_qp](i) * (grad_vel * grad_test);
        }
        _local_re(_i) += raw_value(stabilization * value);
      }
    }
  }
  else
  {
    RealVectorValue regular_grad_test;
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
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
          stabilization(i) = _tau_cd[_qp](i) * (grad_vel * regular_grad_test);
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
  ADRealVectorValue grad_vel;
  ADRealVectorValue stabilization;

  if (_use_displaced_mesh)
  {
    ADRealVectorValue grad_test;
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
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
          stabilization(i) = _tau_cd[_qp](i) * (grad_vel * grad_test);
        }
        _residuals[_i] += stabilization * value;
      }
    }
  }
  else
  {
    RealVectorValue regular_grad_test;
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
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
          stabilization(i) = _tau_cd[_qp](i) * (grad_vel * regular_grad_test);
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
