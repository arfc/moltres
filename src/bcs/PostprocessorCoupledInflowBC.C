#include "PostprocessorCoupledInflowBC.h"

registerMooseObject("MoltresApp", PostprocessorCoupledInflowBC);

template <>
InputParameters
validParams<PostprocessorCoupledInflowBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredCoupledVar("uvel", "x direction velocity");
  params.addCoupledVar("vvel", 0, "y direction velocity");
  params.addCoupledVar("wvel", 0, "z direction velocity");
  params.addRequiredParam<PostprocessorName>(
      "postprocessor", "The postprocessor from which to derive the inlet concentration.");
  params.addParam<Real>("scale", 1, "The amount to scale the postprocessor value by");
  params.addParam<Real>("offset", 0, "The amount to offset the scaled postprocessor by");
  params.addClassDescription("Postprocessor inflow for convection on Navier-Stokes velocity variables");
  return params;
}

PostprocessorCoupledInflowBC::PostprocessorCoupledInflowBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    // velocity variables
    _pp_value(getPostprocessorValue("postprocessor")),
    _scale(getParam<Real>("scale")),
    _offset(getParam<Real>("offset")),
    _vel_x(coupledValue("uvel")),
    _vel_y(coupledValue("vvel")),
    _vel_z(coupledValue("wvel"))
{
}

Real
PostprocessorCoupledInflowBC::computeQpResidual()
{
  Real vdotn = _vel_x[_qp] * _normals[_qp](0) + _vel_y[_qp] * _normals[_qp](1) +
               _vel_z[_qp] * _normals[_qp](2);

  Real inlet_conc = _scale * _pp_value + _offset;

  return _test[_i][_qp] * inlet_conc * vdotn;
}

Real
PostprocessorCoupledInflowBC::computeQpJacobian()
{
  return 0.;
}
