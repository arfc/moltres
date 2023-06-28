#include "SATurbulentViscosityCD.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", SATurbulentViscosityCD);

InputParameters
SATurbulentViscosityCD::validParams()
{
  InputParameters params = ADKernelStabilized::validParams();
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
  ADRealVectorValue value(0., 0., 0.);
  if (std::norm(raw_value(_grad_u[_qp])) > 0.)
    value += _tau_visc_cd[_qp] * (_velocity[_qp] * _grad_u[_qp]) /
             (_grad_u[_qp] * _grad_u[_qp]) * _grad_u[_qp];
//    value += _tau_visc_cd[_qp] * _visc_strong_residual[_qp] /
//             (_grad_u[_qp] * _grad_u[_qp]) * _grad_u[_qp];
  return value;
}

ADReal
SATurbulentViscosityCD::precomputeQpStrongResidual()
{
  return _visc_strong_residual[_qp];
}
