#include "SATimeDerivative.h"

registerMooseObject("MoltresApp", SATimeDerivative);

InputParameters
SATimeDerivative::validParams()
{
  InputParameters params = ADTimeDerivative::validParams();
  params.addClassDescription("Computes the time derivative term in the Spalart-Allmaras "
                             "turbulence model.");
  params.addParam<MaterialPropertyName>(
      "rho_name", "rho", "The name of the density material property");
  return params;
}

SATimeDerivative::SATimeDerivative(const InputParameters & parameters)
  : ADTimeDerivative(parameters),
    _time_strong_residual(getADMaterialProperty<Real>("time_strong_residual"))
{
}

ADReal
SATimeDerivative::precomputeQpResidual()
{
  return _time_strong_residual[_qp];
}
