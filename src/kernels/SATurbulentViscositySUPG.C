#include "SATurbulentViscositySUPG.h"

registerMooseObject("MoltresApp", SATurbulentViscositySUPG);

InputParameters
SATurbulentViscositySUPG::validParams()
{
  InputParameters params = ADKernelSUPG::validParams();
  params.addClassDescription("Adds the SUPG stabilization to the Spalart-Allmaras turbulent "
                             "viscosity equation");
  params.set<MaterialPropertyName>("tau_name") = "tau_viscosity";
  return params;
}

SATurbulentViscositySUPG::SATurbulentViscositySUPG(const InputParameters & parameters)
  : ADKernelSUPG(parameters),
    _visc_strong_residual(getADMaterialProperty<Real>("visc_strong_residual"))
{
}

ADReal
SATurbulentViscositySUPG::precomputeQpStrongResidual()
{
  return _visc_strong_residual[_qp];
}
