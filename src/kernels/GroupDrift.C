#include "GroupDrift.h"

registerMooseObject("MoltresApp", GroupDrift);

InputParameters
GroupDrift::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar("group_drift_var", "The group drift array variable name.");
  params.addParam<bool>("adaptive", false, "Adaptive drift");
  params.addParam<Point>("center", "Center of correction region for reference for adaptive drift");
  return params;
}

GroupDrift::GroupDrift(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _drift_var(coupledArrayValue("group_drift_var")),
    _drift_grad(coupledArrayGradient("group_drift_var")),
    _adaptive(getParam<bool>("adaptive"))
{
  if (_adaptive)
  {
    if (isParamValid("center"))
      _center = getParam<Point>("center");
    else
      paramError("center", "Need center point of correction region for adaptive drift.");
  }
}

Real
GroupDrift::computeQpResidual()
{
  RealEigenVector array_grad_test(3);
  array_grad_test << _grad_test[_i][_qp](0),
                     _grad_test[_i][_qp](1),
                     _grad_test[_i][_qp](2);
  if (_adaptive)
  {
    RealEigenVector drift = _drift_var[_qp];
    Point location = _q_point[_qp] - _center;
    int idx = std::abs(location(0)) > std::abs(location(1)) ? 0 : 1;
    Real sign = location(idx) / std::abs(location(idx));
    if (_drift_var[_qp](idx) * _drift_grad[_qp](idx,idx) * sign > 0)
      drift(idx) = 0.;
    return computeConcentration(_u, _qp) * array_grad_test.transpose() * drift;
  }

//  if (_adaptive)
//  {
//    if ((_drift_var[_qp](0) > 0 && _drift_grad[_qp](0,0) < 0) ||
//        (_drift_var[_qp](0) < 0 && _drift_grad[_qp](0,0) > 0))
//      return computeConcentration(_u, _qp) * array_grad_test.transpose() * _drift_var[_qp];
//    else
//      return 0;
//  }
  return computeConcentration(_u, _qp) * array_grad_test.transpose() * _drift_var[_qp];
}

Real
GroupDrift::computeQpJacobian()
{
  RealEigenVector array_grad_test(3);
  array_grad_test << _grad_test[_i][_qp](0),
                     _grad_test[_i][_qp](1),
                     _grad_test[_i][_qp](2);
  if (_adaptive)
  {
    RealEigenVector drift = _drift_var[_qp];
    Point location = _q_point[_qp] - _center;
    int idx = std::abs(location(0)) > std::abs(location(1)) ? 0 : 1;
    Real sign = location(idx) / std::abs(location(idx));
    if (_drift_var[_qp](idx) * _drift_grad[_qp](idx,idx) * sign > 0)
      drift(idx) = 0.;
    return computeConcentrationDerivative(_u, _phi, _j, _qp) * array_grad_test.transpose() * drift;
  }
//  if (_adaptive)
//  {
//    if ((_drift_var[_qp](0) > 0 && _drift_grad[_qp](0,0) < 0) || 
//        (_drift_var[_qp](0) < 0 && _drift_grad[_qp](0,0) > 0))
//      return computeConcentrationDerivative(_u, _phi, _j, _qp) * array_grad_test.transpose() *
//        _drift_var[_qp];
//    else
//      return 0;
//  }
  return computeConcentrationDerivative(_u, _phi, _j, _qp) * array_grad_test.transpose() *
    _drift_var[_qp];
}
