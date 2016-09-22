#include "InScatter.h"

template<>
InputParameters validParams<InScatter>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<MaterialPropertyName>("sigma_s", "The group in-scatter cross section.");
  params.addRequiredCoupledVar("scattered_group", "The group that is being scattered into the current group.");
  return params;
}


InScatter::InScatter(const InputParameters & parameters) :
    Kernel(parameters),
    _sigma_s(getMaterialProperty<Real>("sigma_s")),
    _v(coupledValue("scattered_group")),
    _v_id(coupled("scattered_group"))
{
}

Real
InScatter::computeQpResidual()
{
  return -_test[_i][_qp] * _sigma_s[_qp] * _v[_qp];
}

Real
InScatter::computeQpJacobian()
{
  return 0.;
}

Real
InScatter::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_id)
  {
    return -_test[_i][_qp] * _sigma_s[_qp] * _phi[_j][_qp];
  }
  else
    return 0.;
}
