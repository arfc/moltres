#include "SATurbulentViscosity.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("MoltresApp", SATurbulentViscosity);

InputParameters
SATurbulentViscosity::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Adds all terms in the turbulent viscosity equation from the "
                             "Spalart-Allmaras model");
//  params.addRequiredCoupledVar("velocity", "The velocity vector variable");
  params.addParam<MaterialPropertyName>("mu_name", "mu",
                                        "The name of the dynamic viscosity material property");
//  params.addParam<MaterialPropertyName>("rho_name", "rho",
//                                        "The name of the density material property");
//  params.addRequiredCoupledVar("wall_distance_var", "Wall distance aux variable name");
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
  Real sigma = 2. / 3.;
  Real cb2 = .622;
  ADReal diffusion_part = 1. / sigma * ((_mu[_qp] + _u[_qp]) * \
                        _grad_u[_qp] * _grad_test[_i][_qp] - cb2 * _grad_u[_qp] * _grad_u[_qp] * \
                        _test[_i][_qp]);
  return (_convection_strong_residual[_qp] + _destruction_strong_residual[_qp] + \
          _source_strong_residual[_qp]) * _test[_i][_qp] + diffusion_part;
}
