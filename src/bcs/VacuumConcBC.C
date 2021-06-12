#include "VacuumConcBC.h"

registerMooseObject("MoltresApp", VacuumConcBC);

InputParameters
VacuumConcBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params += ScalarTransportBase::validParams();
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
