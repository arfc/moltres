#include "VacuumCorrectionBC.h"

registerMooseObject("MoltresApp", VacuumCorrectionBC);

InputParameters
VacuumCorrectionBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar("vacuum_coef_var",
      "The vacuum boundary coefficient variable name.");
  return params;
}

VacuumCorrectionBC::VacuumCorrectionBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    ScalarTransportBase(parameters),
    _coef_var(coupledValue("vacuum_coef_var"))
{
}

Real
VacuumCorrectionBC::computeQpResidual()
{
  return _test[_i][_qp] * _coef_var[_qp] * computeConcentration(_u, _qp);
}

Real
VacuumCorrectionBC::computeQpJacobian()
{
  return _test[_i][_qp] * _coef_var[_qp] * computeConcentrationDerivative(_u, _phi, _j, _qp);
}
