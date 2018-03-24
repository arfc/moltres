#include "INSBoussinesqBodyForce.h"

registerMooseObject("MoltresApp", INSBoussinesqBodyForce);

template <>
InputParameters
validParams<INSBoussinesqBodyForce>()
{
  InputParameters params = validParams<Kernel>();

  params.addClassDescription("Computes a body force for natural convection buoyancy.");

  // Coupled variables
  params.addRequiredCoupledVar("dT",
                               "the aux variable that gives delta T in Boussinesq body force");
  params.addRequiredCoupledVar("temperature", "temperature variable, for off diagonal jacobian");

  // Required parameters
  params.addRequiredParam<RealVectorValue>("gravity", "Direction of the gravity vector");
  params.addRequiredParam<unsigned>(
      "component",
      "0,1,2 depending on if we are solving the x,y,z component of the momentum equation");

  // Optional parameters
  params.addParam<MaterialPropertyName>("alpha_name",
                                        "alpha",
                                        "The name of the thermal expansion coefficient"
                                        "this is of the form rho = rho*(1-alpha dT)");
  params.addParam<MaterialPropertyName>("rho_name", "rho", "The name of the density");
  return params;
}

INSBoussinesqBodyForce::INSBoussinesqBodyForce(const InputParameters & parameters)
  : Kernel(parameters),

    // variable for delta T
    _dT(coupledValue("dT")),
    _dT_id(coupled("temperature")),

    // Required parameters
    _gravity(getParam<RealVectorValue>("gravity")),
    _component(getParam<unsigned>("component")),

    // Material properties
    _alpha(getMaterialProperty<Real>("alpha_name")),
    _rho(getMaterialProperty<Real>("rho_name"))
{
}

Real
INSBoussinesqBodyForce::computeQpResidual()
{
  return _test[_i][_qp] * _alpha[_qp] * _gravity(_component) * _rho[_qp] * _dT[_qp];
}

Real
INSBoussinesqBodyForce::computeQpJacobian()
{
  return 0;
}

Real
INSBoussinesqBodyForce::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _dT_id)
    // not a perfect one, this could include d alpha dT and d rho dT
    // but those oughta be small
    return _test[_i][_qp] * _alpha[_qp] * _gravity(_component) * _rho[_qp] * _phi[_j][_qp];
  else
    return 0;
}
