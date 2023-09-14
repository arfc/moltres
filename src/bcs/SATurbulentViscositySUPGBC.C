#include "SATurbulentViscositySUPGBC.h"

registerMooseObject("MoltresApp", SATurbulentViscositySUPGBC);

InputParameters
SATurbulentViscositySUPGBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addClassDescription("This class removes SUPG contributions in the Spalart-Allmaras"
                             " equation along the boundary.");
  params.addParam<MaterialPropertyName>("tau_name", "tau_viscosity",
                                        "The name of the dynamic viscosity material property");
  params.addRequiredCoupledVar("velocity", "The velocity variable.");
  return params;
}

SATurbulentViscositySUPGBC::SATurbulentViscositySUPGBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _tau_visc(getADMaterialProperty<Real>("tau_name")),
    _velocity(adCoupledVectorValue("velocity")),
    _visc_strong_residual(getADMaterialProperty<Real>("visc_strong_residual"))
{
}

ADRealVectorValue
SATurbulentViscositySUPGBC::computeQpStabilization()
{
  return _velocity[_qp] * _tau_visc[_qp];
}

ADReal
SATurbulentViscositySUPGBC::precomputeQpStrongResidual()
{
  return -_visc_strong_residual[_qp];
}

void
SATurbulentViscositySUPGBC::computeResidual()
{
  _residuals.resize(_test.size(), 0);
  for (auto & r : _residuals)
    r = 0;

  if (_use_displaced_mesh)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < _test.size(); _i++)
        _residuals[_i] += raw_value(_test[_i][_qp] * _normals[_qp] * computeQpStabilization() *
            value);
    }
  else
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < _test.size(); _i ++)
        _residuals[_i] +=
          raw_value((_test[_i][_qp] * _normals[_qp]) * computeQpStabilization() * value);
    }

  addResiduals(_assembly, _residuals, _var.dofIndices(), _var.scalingFactor());

  if (_has_save_in)
    for (unsigned int i = 0; i < _save_in.size(); i++)
      _save_in[i]->sys().solution().add_vector(_residuals.data(), _save_in[i]->dofIndices());
}

void
SATurbulentViscositySUPGBC::computeResidualsForJacobian()
{
  if (_residuals_and_jacobians.size() != _test.size())
    _residuals_and_jacobians.resize(_test.size(), 0);
  for (auto & r : _residuals_and_jacobians)
    r = 0;

  if (_use_displaced_mesh)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _ad_JxW[_qp] * _ad_coord[_qp];
      for (_i = 0; _i < _test.size(); _i++)
        _residuals_and_jacobians[_i] += _test[_i][_qp] * _normals[_qp] * computeQpStabilization() *
          value;
    }
  else
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      const auto value = precomputeQpStrongResidual() * _JxW[_qp] * _coord[_qp];
      for (_i = 0; _i < _test.size(); _i ++)
        _residuals_and_jacobians[_i] +=
          (_test[_i][_qp] * _normals[_qp]) * computeQpStabilization() * value;
    }
}

ADReal
SATurbulentViscositySUPGBC::computeQpResidual()
{
  mooseError("SATurbulentViscositySUPGBC should not use computeQpResidual.");
}
