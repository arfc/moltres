#include "CoupledFissionEigenKernel.h"

template<>
InputParameters validParams<CoupledFissionEigenKernel>()
{
  InputParameters params = validParams<EigenKernel>();
  params.addRequiredParam<MaterialPropertyName>("nu_f", "The number of neutrons produced per fission.");
  params.addRequiredParam<MaterialPropertyName>("sigma_f", "The macroscopic fission cross section.");
  params.addRequiredCoupledVar("fissioning_group", "The group that is fissioning.");
  return params;
}

CoupledFissionEigenKernel::CoupledFissionEigenKernel(const InputParameters & parameters) :
    EigenKernel(parameters),
    _v(coupledValue("fissioning_group")),
    _v_id(coupled("fissioning_group")),
    _nu_f(getMaterialProperty<Real>("nu_f")),
    _sigma_f(getMaterialProperty<Real>("sigma_f"))
{
}

Real
CoupledFissionEigenKernel::computeQpResidual()
{
  return -_nu_f[_qp] * _sigma_f[_qp] * _v[_qp] * _test[_i][_qp];
}

Real
CoupledFissionEigenKernel::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_id)
  {
    return -_nu_f[_qp] * _sigma_f[_qp] * _phi[_j][_qp] * _test[_i][_qp];
  }

  else
    return 0.;
}
