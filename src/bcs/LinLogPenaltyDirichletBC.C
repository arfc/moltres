#include "LinLogPenaltyDirichletBC.h"
#include "Function.h"

template <>
InputParameters
validParams<LinLogPenaltyDirichletBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<Real>("penalty", "LinLogPenalty scalar");
  params.addParam<Real>("value", 0.0, "Boundary value of the variable");

  return params;
}

LinLogPenaltyDirichletBC::LinLogPenaltyDirichletBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    ScalarTransportBase(parameters),
    _p(getParam<Real>("penalty")),
    _v(getParam<Real>("value"))
{
}

Real
LinLogPenaltyDirichletBC::computeQpResidual()
{
  return _p * _test[_i][_qp] * (-_v + computeConcentration(_u, _qp));
}

Real
LinLogPenaltyDirichletBC::computeQpJacobian()
{
  return _p * computeConcentrationDerivative(_u, _phi, _j, _qp) * _test[_i][_qp];
}
