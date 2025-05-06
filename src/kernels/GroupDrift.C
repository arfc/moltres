#include "GroupDrift.h"

registerMooseObject("MoltresApp", GroupDrift);

InputParameters
GroupDrift::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar("group_drift_var", "The group drift array variable name.");
  params.addParam<bool>("adaptive", false, "Adaptive drift");
  params.addParam<Point>("bottom_left",
      "The bottom-left point of the interior of the adaptive drift region");
  params.addParam<Point>("top_right",
      "The top-right point of the interior of the adaptive drift region");
  params.addParam<bool>("use_jacobian", true, "Whether to compute the Jacobian for preconditioning");
  return params;
}

GroupDrift::GroupDrift(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _drift_var(coupledArrayValue("group_drift_var")),
    _drift_grad(coupledArrayGradient("group_drift_var")),
    _adaptive(getParam<bool>("adaptive")),
    _use_jacobian(getParam<bool>("use_jacobian"))
{
  if (_adaptive)
  {
    _bot_left = getParam<Point>("bottom_left");
    _top_right = getParam<Point>("top_right");
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
    Real tol = 1e-8;
    Real z_coord = _q_point[_qp](2);
    Real sign;
    if (z_coord < _bot_left(2) - tol || z_coord > _top_right(2) + tol)
    {
      sign = z_coord < _bot_left(2) - tol ? -1.0 : 1.0;
      if (_drift_var[_qp](2) * _drift_grad[_qp](2,2) * sign > 0)
        return 0.0;
    }
    else
    {
      Point center = (_bot_left + _top_right) / 2.0;
      Point location = _q_point[_qp] - center;
      int idx = std::abs(location(0)) > std::abs(location(1)) ? 0 : 1;
      sign = location(idx) / std::abs(location(idx));
      if (_drift_var[_qp](idx) * _drift_grad[_qp](idx,idx) * sign > 0)
        return 0.0;
    }
  }
  return computeConcentration(_u, _qp) * array_grad_test.transpose() * _drift_var[_qp];
}

Real
GroupDrift::computeQpJacobian()
{
  if (!(_use_jacobian))
    return 0;
  RealEigenVector array_grad_test(3);
  array_grad_test << _grad_test[_i][_qp](0),
                     _grad_test[_i][_qp](1),
                     _grad_test[_i][_qp](2);
  if (_adaptive)
  {
    Real tol = 1e-8;
    Real z_coord = _q_point[_qp](2);
    Real sign;
    if (z_coord < _bot_left(2) - tol || z_coord > _top_right(2) + tol)
    {
      sign = z_coord < _bot_left(2) - tol ? -1.0 : 1.0;
      if (_drift_var[_qp](2) * _drift_grad[_qp](2,2) * sign > 0)
        return 0.0;
    }
    else
    {
      Point center = (_bot_left + _top_right) / 2.0;
      Point location = _q_point[_qp] - center;
      int idx = std::abs(location(0)) > std::abs(location(1)) ? 0 : 1;
      sign = location(idx) / std::abs(location(idx));
      if (_drift_var[_qp](idx) * _drift_grad[_qp](idx,idx) * sign > 0)
        return 0.0;
    }
  }
  return computeConcentrationDerivative(_u, _phi, _j, _qp) * array_grad_test.transpose() *
    _drift_var[_qp];
}
