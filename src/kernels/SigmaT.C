#include "SigmaT.h"

template<>
InputParameters validParams<SigmaT>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<MaterialPropertyName>("sigma_t", "The total macrosopic loss cross-section.");
  return params;
}


SigmaT::SigmaT(const InputParameters & parameters) :
    Kernel(parameters),
    _sigma_t(getMaterialProperty<Real>("sigma_t"))
{
}

Real
SigmaT::computeQpResidual()
{
  return _test[_i][_qp] * _sigma_t[_qp] * _u[_qp];
}

Real
SigmaT::computeQpJacobian()
{
  return _test[_i][_qp] * _sigma_t[_qp] * _phi[_j][_qp];
}
