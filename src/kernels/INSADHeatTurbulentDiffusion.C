#include "INSADHeatTurbulentDiffusion.h"

#include "metaphysicl/raw_type.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", INSADHeatTurbulentDiffusion);

InputParameters
INSADHeatTurbulentDiffusion::validParams()
{
  InputParameters params = ADDiffusion::validParams();
  params.addClassDescription("Adds the turbulent diffusion term in the INSAD heat equation");
  params.addParam<MaterialPropertyName>("cp_name", "cp", "The name of the specific heat capacity");
  params.addParam<MaterialPropertyName>("mu_name", "mu",
                                        "The name of the dynamic viscosity material property");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  params.addParam<Real>("prandtl_number", 0.85, "Prandtl number");
  return params;
}

INSADHeatTurbulentDiffusion::INSADHeatTurbulentDiffusion(const InputParameters & parameters)
  : ADDiffusion(parameters),
    _cp(getADMaterialProperty<Real>("cp_name")),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _mu_tilde(adCoupledValue("mu_tilde")),
    _pr(getParam<Real>("prandtl_number"))
{
}

ADRealVectorValue
INSADHeatTurbulentDiffusion::precomputeQpResidual()
{
  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  Real cv1 = 7.1;
  ADReal fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));

  return _cp[_qp] * _mu_tilde[_qp] * fv1 / _pr * ADDiffusion::precomputeQpResidual();
}
