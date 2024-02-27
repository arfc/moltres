#include "SATurbulentViscosityAux.h"
#include "metaphysicl/raw_type.h"

registerMooseObject("MoltresApp", SATurbulentViscosityAux);

InputParameters
SATurbulentViscosityAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes the turbulent viscosity for the Spalart-Allmaras "
                             "turbulence model.");
  params.addRequiredCoupledVar("mu_tilde",
                               "Scaled turbulent viscosity variable from the Spalart-Allmaras "
                               "transport equation.");
  params.addParam<MaterialPropertyName>(
      "mu_name", "mu", "The name of the viscosity material property");
  return params;
}

SATurbulentViscosityAux::SATurbulentViscosityAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _mu_tilde(coupledValue("mu_tilde")),
    _mu(getADMaterialProperty<Real>("mu_name"))
{
}

Real
SATurbulentViscosityAux::computeValue()
{
  Real chi = _mu_tilde[_qp] / MetaPhysicL::raw_value(_mu[_qp]);
  Real cv1 = 7.1;
  Real fv1 = std::pow(chi, 3) / (std::pow(chi, 3) + std::pow(cv1, 3));
  return _mu_tilde[_qp] * fv1;
}
