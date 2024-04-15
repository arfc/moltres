#include "GroupDrift.h"

registerMooseObject("MoltresApp", GroupDrift);

InputParameters
GroupDrift::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar("group_drift_var", "The group drift array variable name.");
  return params;
}

GroupDrift::GroupDrift(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _drift_var(coupledArrayValue("group_drift_var"))
{
}

Real
GroupDrift::computeQpResidual()
{
  RealEigenVector array_grad_test(3);
  array_grad_test << _grad_test[_i][_qp](0),
                     _grad_test[_i][_qp](1),
                     _grad_test[_i][_qp](2);
  return (computeConcentration(_u, _qp) * array_grad_test.transpose() * _drift_var[_qp]);
}
