#include "SelfFissionEigenKernel.h"

registerMooseObject("MoltresApp", SelfFissionEigenKernel);

InputParameters
SelfFissionEigenKernel::validParams()
{
  InputParameters params = EigenKernel::validParams();
  params.addRequiredParam<MaterialPropertyName>("nu_f",
                                                "The number of neutrons produced per fission.");
  params.addRequiredParam<MaterialPropertyName>("sigma_f",
                                                "The macroscopic fission cross section.");
  return params;
}

SelfFissionEigenKernel::SelfFissionEigenKernel(const InputParameters & parameters)
  : EigenKernel(parameters),
    _nu_f(getMaterialProperty<Real>("nu_f")),
    _sigma_f(getMaterialProperty<Real>("sigma_f"))
{
}

Real
SelfFissionEigenKernel::computeQpResidual()
{
  return -_nu_f[_qp] * _sigma_f[_qp] * _u[_qp] * _test[_i][_qp];
}

Real
SelfFissionEigenKernel::computeQpJacobian()
{
  return -_nu_f[_qp] * _sigma_f[_qp] * _phi[_j][_qp] * _test[_i][_qp];
}
