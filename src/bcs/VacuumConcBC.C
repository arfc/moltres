#include "VacuumConcBC.h"

registerMooseObject("MoltresApp", VacuumConcBC);

InputParameters
VacuumConcBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params += ScalarTransportBase::validParams();
  MooseEnum vacuum_bc_type("marshak mark milne", "marshak");
  params.addParam<MooseEnum>("vacuum_bc_type", vacuum_bc_type,
      "Whether to apply Marshak, Mark, or Milne vacuum boundary conditions. Defaults to Marshak.");
  return params;
}

VacuumConcBC::VacuumConcBC(const InputParameters & parameters)
  : IntegratedBC(parameters), ScalarTransportBase(parameters)
{
  switch (getParam<MooseEnum>("vacuum_bc_type"))
  {
    case MARSHAK:
      _alpha = 2.;
      break;
    case MARK:
      _alpha = std::sqrt(3.);
      break;
    case MILNE:
      _alpha = 3 * 0.710446;
      break;
  }
}

Real
VacuumConcBC::computeQpResidual()
{
  return _test[_i][_qp] * computeConcentration(_u, _qp) / _alpha;
}

Real
VacuumConcBC::computeQpJacobian()
{
  return _test[_i][_qp] * computeConcentrationDerivative(_u, _phi, _j, _qp) / _alpha;
}
