#include "INSADMomentumTurbulentNoBCBC.h"

registerMooseObject("MoltresApp", INSADMomentumTurbulentNoBCBC);

InputParameters
INSADMomentumTurbulentNoBCBC::validParams()
{
  InputParameters params = INSADMomentumNoBCBC::validParams();
  params.addClassDescription("This class implements the 'No BC' boundary condition with "
                             "additional contributions from turbulent viscosity.");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  return params;
}

INSADMomentumTurbulentNoBCBC::INSADMomentumTurbulentNoBCBC(const InputParameters & parameters)
  : INSADMomentumNoBCBC(parameters), _mu_tilde(adCoupledValue("mu_tilde"))
{
  if (_form == "laplace")
    mooseError("The Laplace form of the INS equations is incompatible with the Spalart-Allmaras "
               "turbulent viscosity variable.");
}

ADReal
INSADMomentumTurbulentNoBCBC::computeQpResidual()
{
  using std::pow;
  ADReal chi = _mu_tilde[_qp] / _mu[_qp];
  ADReal fv1 = pow(chi, 3) / (pow(chi, 3) + pow(7.1, 3));
  ADReal residual;
  residual = -(_mu[_qp] + _mu_tilde[_qp] * fv1) *
             ((_grad_u[_qp] + _grad_u[_qp].transpose()) * _normals[_qp]) * _test[_i][_qp];
  if (_integrate_p_by_parts)
    residual += _p[_qp] * _normals[_qp] * _test[_i][_qp];
  return residual;
}
