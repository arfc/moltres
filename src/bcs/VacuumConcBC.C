#include "VacuumConcBC.h"

template<>
InputParameters validParams<VacuumConcBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params += validParams<ScalarTransportBase>();
  params.addParam<Real>("alpha", 1, "No idea.");
  return params;
}

VacuumConcBC::VacuumConcBC(const InputParameters & parameters) :
    IntegratedBC(parameters),
    ScalarTransportBase(parameters),
    _alpha(getParam<Real>("alpha"))
{}

Real
VacuumConcBC::computeQpResidual()
{
  return _test[_i][_qp] * _alpha * computeConcentration(_u, _qp) / 2.;
}

Real
VacuumConcBC::computeQpJacobian()
{
  return _test[_i][_qp] * _alpha * computeConcentrationDerivative(_u, _phi, _j, _qp) / 2.;
}
