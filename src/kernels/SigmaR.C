#include "SigmaR.h"

template<>
InputParameters validParams<SigmaR>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<MaterialPropertyName>("sigma_r", "The total macrosopic loss cross-section.");
  return params;
}


SigmaR::SigmaR(const InputParameters & parameters) :
    Kernel(parameters),
    _sigma_r(getMaterialProperty<Real>("sigma_r"))
{
}

Real
SigmaR::computeQpResidual()
{
  return _test[_i][_qp] * _sigma_r[_qp] * _u[_qp];
}

Real
SigmaR::computeQpJacobian()
{
  return _test[_i][_qp] * _sigma_r[_qp] * _phi[_j][_qp];
}
