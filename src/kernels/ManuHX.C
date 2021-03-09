#include "ManuHX.h"

registerMooseObject("MoltresApp", ManuHX);

InputParameters
ManuHX::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("A uniformly applied heat removal proportional to a difference"
                             "from a nominal temperature. A global heat exchanger.");
  params.addRequiredParam<Real>("htc", "heat transfer coefficient for q'=h*(T-T_ref)");
  params.addRequiredParam<Real>("tref", "reference temperature");
  return params;
}

ManuHX::ManuHX(const InputParameters & parameters)
  : Kernel(parameters),
    _htc(getParam<Real>("htc")),
    _tref(getParam<Real>("tref"))
{
}

Real
ManuHX::computeQpResidual()
{
  // positive -> heat removal
  return _test[_i][_qp] * _htc * (_u[_qp] - _tref);
}

Real
ManuHX::computeQpJacobian()
{
  return _test[_i][_qp] * _htc * _phi[_j][_qp];
}
