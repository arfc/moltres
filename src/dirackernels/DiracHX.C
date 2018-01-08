#include "DiracHX.h"

template <>
InputParameters
validParams<DiracHX>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addRequiredParam<Real>("power", "HX Power in Watts");
  params.addRequiredParam<Point>("point", "The x,y,z coordinates of the point");
  params.declareControllable("power");
  return params;
}

DiracHX::DiracHX(const InputParameters & parameters)
  : DiracKernel(parameters), _power(getParam<Real>("power")), _point(getParam<Point>("point"))
{
}

void
DiracHX::addPoints()
{
  // Add a point from the input file
  addPoint(_point);
}

Real
DiracHX::computeQpResidual()
{
  // positive term -> heat removal
  return _test[_i][_qp] * _power;
}
