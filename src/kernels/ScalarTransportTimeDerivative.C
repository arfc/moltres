#include "ScalarTransportTimeDerivative.h"

template<>
InputParameters validParams<ScalarTransportTimeDerivative>()
{
  InputParameters params = validParams<TimeKernel>();
  params += ScalarTransportBase<TimeKernel>::validParams();
  params.addParam<bool>("lumping", false, "True for mass matrix lumping, false otherwise");
  return params;
}

ScalarTransportTimeDerivative::ScalarTransportTimeDerivative(const InputParameters & parameters) :
    ScalarTransportBase<TimeKernel>(parameters),
    _lumping(getParam<bool>("lumping"))

{
}

Real
ScalarTransportTimeDerivative::computeQpResidual()
{
  return _test[_i][_qp] * computeConcentrationDot();
}

Real
ScalarTransportTimeDerivative::computeQpJacobian()
{
  return _test[_i][_qp] * computeConcentrationDotDerivative();
}
