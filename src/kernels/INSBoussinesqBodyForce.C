#include "INSBoussinesqBodyForce.h"

registerMooseObject("MoltresApp", INSBoussinesqBodyForce);

template <>
InputParameters
validParams<INSBoussinesqBodyForce>()
{
  InputParameters params = validParams<Kernel>();

  params.addClassDescription("Computes a body force for natural convection buoyancy.");

  // Coupled variables
  params.addRequiredCoupledVar("temperature", "temperature variable, for off diagonal jacobian");

  // Required parameters
  params.addRequiredParam<RealVectorValue>("gravity", "Direction of the gravity vector");
  params.addRequiredParam<unsigned>(
      "component",
      "0,1,2 depending on if we are solving the x,y,z component of the momentum equation");
  params.addParam<MaterialPropertyName>("alpha_name",
                                        "alpha",
                                        "The name of the thermal expansion coefficient"
                                        "this is of the form rho = rho*(1-alpha (T-T_ref))");
  params.addParam<MaterialPropertyName>("rho_name", "rho", "The name of the density");
  params.addParam<MaterialPropertyName>(
      "ref_temp", "temp_ref", "The name of the reference temperature");
  return params;
}

INSBoussinesqBodyForce::INSBoussinesqBodyForce(const InputParameters & parameters)
  : Kernel(parameters),

    // variable for delta T
    _T(coupledValue("temperature")),
    _T_id(coupled("temperature")),

    // Required parameters
    _gravity(getParam<RealVectorValue>("gravity")),
    _component(getParam<unsigned>("component")),

    // Material properties
    _alpha(getMaterialProperty<Real>("alpha_name")),
    _rho(getMaterialProperty<Real>("rho_name")),
    _T_ref(getMaterialProperty<Real>("ref_temp"))
{
}

Real
INSBoussinesqBodyForce::computeQpResidual()
{
  return _test[_i][_qp] * _alpha[_qp] * _gravity(_component) * _rho[_qp] * (_T[_qp] - _T_ref[_qp]);
}

Real
INSBoussinesqBodyForce::computeQpJacobian()
{
  return 0;
}

Real
INSBoussinesqBodyForce::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _T_id)
    return _test[_i][_qp] * _alpha[_qp] * _gravity(_component) * _rho[_qp] * _phi[_j][_qp];
  else
    return 0;
}
