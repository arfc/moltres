#include "DivFreeCoupledScalarAdvection.h"
#include "MooseMesh.h"

template <>
InputParameters
validParams<DivFreeCoupledScalarAdvection>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();

  // DivFreeCoupled variables
  params.addCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", "y-velocity");
  params.addCoupledVar("w", "z-velocity");
  params.addParam<Real>(
      "u_def", 0, "Allows user to specify constant value for u component of velocity.");
  params.addParam<Real>(
      "v_def", 0, "Allows user to specify constant value for v component of velocity.");
  params.addParam<Real>(
      "w_def", 0, "Allows user to specify constant value for w component of velocity.");
  params.addParam<Real>(
      "conc_scaling", 1, "The amount by which to scale the concentration variable.");
  return params;
}

DivFreeCoupledScalarAdvection::DivFreeCoupledScalarAdvection(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),

    // DivFreeCoupled variables
    _u_vel(isCoupled("u") ? coupledValue("u") : _u_def),
    _v_vel(isCoupled("v") ? coupledValue("v") : _v_def),
    _w_vel(isCoupled("w") ? coupledValue("w") : _w_def),

    // Variable numberings
    _u_vel_var_number(coupled("u")),
    _v_vel_var_number(coupled("v")),
    _w_vel_var_number(coupled("w")),
    _conc_scaling(getParam<Real>("conc_scaling"))
{
  if (!(isCoupled("u")))
    _u_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("u_def")));
  if (!(isCoupled("v")))
    _v_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("v_def")));
  if (!(isCoupled("w")))
    _w_def.resize(_fe_problem.getMaxQps(), Real(getParam<Real>("w_def")));
}

Real
DivFreeCoupledScalarAdvection::computeQpResidual()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  return computeConcentrationGradient(_u, _grad_u, _qp) * U * _test[_i][_qp] * _conc_scaling;
}

Real
DivFreeCoupledScalarAdvection::computeQpJacobian()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  return computeConcentrationGradientDerivative(_u, _grad_u, _phi, _grad_phi, _j, _qp) * U *
         _test[_i][_qp] * _conc_scaling;
}

Real
DivFreeCoupledScalarAdvection::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _u_vel_var_number)
    return computeConcentrationGradient(_u, _grad_u, _qp)(0) * _phi[_j][_qp] * _test[_i][_qp] *
           _conc_scaling;

  else if (jvar == _v_vel_var_number)
    return computeConcentrationGradient(_u, _grad_u, _qp)(1) * _phi[_j][_qp] * _test[_i][_qp] *
           _conc_scaling;

  else if (jvar == _w_vel_var_number)
    return computeConcentrationGradient(_u, _grad_u, _qp)(2) * _phi[_j][_qp] * _test[_i][_qp] *
           _conc_scaling;

  else
    return 0;
}
