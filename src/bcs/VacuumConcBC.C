#include "VacuumConcBC.h"

template <>
InputParameters
validParams<VacuumConcBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params += validParams<ScalarTransportBase>();
  return params;
}

VacuumConcBC::VacuumConcBC(const InputParameters & parameters)
  : IntegratedBC(parameters), ScalarTransportBase(parameters)
{
}

Real
VacuumConcBC::computeQpResidual()
{
  return _test[_i][_qp] * computeConcentration(_u, _qp) / 2.;
}

Real
VacuumConcBC::computeQpJacobian()
{
  return _test[_i][_qp] * computeConcentrationDerivative(_u, _phi, _j, _qp) / 2.;
}
