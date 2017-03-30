#include "MatINSTemperatureRZ.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<MatINSTemperatureRZ>()
{
  InputParameters params = validParams<Kernel>();

  // Coupled variables
  params.addCoupledVar("ur", 0, "Radial velocity");
  params.addCoupledVar("uz", 0, "Axial velocity");
  // params.addParam<MaterialPropertyName>("rho", "The material property name for the density");
  // params.addParam<MaterialPropertyName>("k", "The material property name for the thermal conductivity");
  // params.addParam<MaterialPropertyName>("cp", "The material property name for the specific heat");

  return params;
}

MatINSTemperatureRZ::MatINSTemperatureRZ(const InputParameters & parameters) :
    Kernel(parameters),

    // Coupled variables
    _ur(coupledValue("ur")),
    _uz(coupledValue("uz")),

    // Variable numberings
    _ur_var_number(coupled("ur")),
    _uz_var_number(coupled("uz")),

    // Required parameters
    _rho(getMaterialProperty<Real>("rho")),
    _k(getMaterialProperty<Real>("k")),
    _cp(getMaterialProperty<Real>("cp"))
{
  if (!(_mesh.dimension() == 2))
    mooseError("You're definitely not running an RZ simulation.");
}



Real MatINSTemperatureRZ::computeQpResidual()
{
  // The convection part, rho * cp u.grad(T) * v.
  // Note: _u is the temperature variable, _grad_u is its gradient.
  Real convective_part = _rho[_qp] * _cp[_qp] *
    (_ur[_qp]*_grad_u[_qp](0) +
     _uz[_qp]*_grad_u[_qp](1)) * _test[_i][_qp];

  // Thermal conduction part, k * grad(T) * grad(v)
  Real conduction_part = _k[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];

  return convective_part + conduction_part;
}




Real MatINSTemperatureRZ::computeQpJacobian()
{
  RealVectorValue U(_ur[_qp], _uz[_qp], 0);

  Real convective_part = _rho[_qp] * _cp[_qp] * (U * _grad_phi[_j][_qp]) * _test[_i][_qp];
  Real conduction_part = _k[_qp] * (_grad_phi[_j][_qp] * _grad_test[_i][_qp]);

  return convective_part + conduction_part;
}




Real MatINSTemperatureRZ::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _ur_var_number)
  {
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](0) * _test[_i][_qp];
    return convective_part;
  }

  else if (jvar == _uz_var_number)
  {
    Real convective_part = _phi[_j][_qp] * _grad_u[_qp](1) * _test[_i][_qp];
    return convective_part;
  }

  else
    return 0;
}
