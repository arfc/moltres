#include "INSADMomentumSUPGBC.h"
#include "Assembly.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", INSADMomentumSUPGBC);

InputParameters
INSADMomentumSUPGBC::validParams()
{
  InputParameters params = ADVectorIntegratedBC::validParams();
  params.addClassDescription("This class removes INS SUPG contributions along the boundary.");
  params.addParam<MaterialPropertyName>(
      "rho_name", "rho", "Density");
  params.addParam<MaterialPropertyName>(
      "tau_name", "tau", "The name of the stabilization parameter tau.");
  return params;
}

INSADMomentumSUPGBC::INSADMomentumSUPGBC(const InputParameters & parameters)
  : ADVectorIntegratedBC(parameters),
    _rho(getADMaterialProperty<Real>("rho_name")),
    _tau(getADMaterialProperty<Real>("tau_name")),
    _momentum_strong_residual(getADMaterialProperty<RealVectorValue>("momentum_strong_residual"))
{
}

ADRealVectorValue
INSADMomentumSUPGBC::computeQpStabilization()
{
  return _u[_qp] * _tau[_qp];
}

ADRealVectorValue
INSADMomentumSUPGBC::precomputeQpStrongResidual()
{
  return -_momentum_strong_residual[_qp];
}

void
INSADMomentumSUPGBC::computeResidual()
{
  _residuals.resize(_grad_test.size(), 0);
  for (auto & r : _residuals)
    r = 0;

  if (_use_displaced_mesh)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i++)
        _residuals[_i] += raw_value(_grad_test[_i][_qp] * computeQpStabilization() * value);
    }
  else
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i ++)
        _residuals[_i] +=
          raw_value((_test[_i][_qp] * _normals[_qp]) * computeQpStabilization() * value);
    }

  _assembly.processResiduals(_residuals, _var.dofIndices(), _vector_tags, _var.scalingFactor());

  if (_has_save_in)
    for (unsigned int i = 0; i < _save_in.size(); i++)
      _save_in[i]->sys().solution().add_vector(_residuals.data(), _save_in[i]->dofIndices());
}

void
INSADMomentumSUPGBC::computeResidualsForJacobian()
{
  if (_residuals_and_jacobians.size() != _grad_test.size())
    _residuals_and_jacobians.resize(_grad_test.size(), 0);
  for (auto & r : _residuals_and_jacobians)
    r = 0;

  if (_use_displaced_mesh)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i++)
        _residuals_and_jacobians[_i] += _grad_test[_i][_qp] * computeQpStabilization() * value;
    }
  else
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < _grad_test.size(); _i ++)
        _residuals_and_jacobians[_i] +=
          (_test[_i][_qp] * _normals[_qp]) * computeQpStabilization() * value;
    }
}

ADReal
INSADMomentumSUPGBC::computeQpResidual()
{
  mooseError("INSADMomentumSUPGBC should not use computeQpResidual.");
}
