#include "SATurbulentViscosityNoBCBC.h"

registerMooseObject("MoltresApp", SATurbulentViscosityNoBCBC);

InputParameters
SATurbulentViscosityNoBCBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addClassDescription("This class implements the 'No BC' boundary condition for the "
                             "Spalart-Allmaras equation");
  params.addParam<MaterialPropertyName>("mu_name", "mu", "The name of the dynamic viscosity");
  return params;
}

SATurbulentViscosityNoBCBC::SATurbulentViscosityNoBCBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _mu(getADMaterialProperty<Real>("mu_name"))
{
}

ADReal
SATurbulentViscosityNoBCBC::computeQpResidual()
{
  Real sigma = 2. / 3.;
  return -1. / sigma * (_mu[_qp] + _u[_qp]) * (_grad_u[_qp] * _normals[_qp]) * _test[_i][_qp];
}
