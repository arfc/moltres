#include "TurbulentStressAux.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", TurbulentStressAux);

InputParameters
TurbulentStressAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes the turbulent stress introduced by the Spalart-Allmaras "
                             "model.");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  params.addParam<MaterialPropertyName>(
      "mu_name", "mu", "The name of the viscosity material property");
  params.addParam<MaterialPropertyName>(
      "rho_name", "rho", "The name of the density material property");
  return params;
}

TurbulentStressAux::TurbulentStressAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _mu_tilde(adCoupledValue("mu_tilde")),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _rho(getADMaterialProperty<Real>("rho_name")),
    _strain_mag(getADMaterialProperty<Real>("strain_mag"))
{
}

Real
TurbulentStressAux::computeValue()
{
  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  ADReal fv1 = std::pow(chi, 3) / (std::pow(chi, 3) + std::pow(7.1, 3));
  ADReal nu_T = _mu_tilde[_qp] * fv1 / _rho[_qp];
  // Divide by sqrt(2) because _strain_mag = sqrt(2 * S_ij * S_ij)
  return raw_value(std::sqrt(2.) * nu_T * _strain_mag[_qp]);
}
