#include "SATurbulentViscosity.h"

registerMooseObject("MoltresApp", SATurbulentViscosity);

InputParameters
SATurbulentViscosity::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Adds all terms in the turbulent viscosity equation from the "
                             "Spalart-Allmaras model");
  params.addParam<MaterialPropertyName>("mu_name", "mu",
                                        "The name of the dynamic viscosity material property");
  return params;
}

SATurbulentViscosity::SATurbulentViscosity(const InputParameters & parameters)
  : ADKernel(parameters),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _convection_strong_residual(getADMaterialProperty<Real>("convection_strong_residual")),
    _destruction_strong_residual(getADMaterialProperty<Real>("destruction_strong_residual")),
    _diffusion_strong_residual(getADMaterialProperty<Real>("diffusion_strong_residual")),
    _source_strong_residual(getADMaterialProperty<Real>("source_strong_residual"))
{
}

ADReal
SATurbulentViscosity::computeQpResidual()
{
  Real sigma = 2.0 / 3.0;
  Real cb2 = 0.622;
  ADReal diffusion_part = 1.0 / sigma * ((_mu[_qp] + _u[_qp]) * \
                        _grad_u[_qp] * _grad_test[_i][_qp] - cb2 * _grad_u[_qp] * _grad_u[_qp] * \
                        _test[_i][_qp]);
  return (_convection_strong_residual[_qp] + _destruction_strong_residual[_qp] + \
          _source_strong_residual[_qp]) * _test[_i][_qp] + diffusion_part;
}
