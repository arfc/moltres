#include "INSADMassPSPGBC.h"

registerMooseObject("MoltresApp", INSADMassPSPGBC);

InputParameters
INSADMassPSPGBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addClassDescription("This class removes INS PSPG contributions along the boundary.");
  params.addParam<MaterialPropertyName>("rho_name", "rho", "The name of the density");
  return params;
}

INSADMassPSPGBC::INSADMassPSPGBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _rho(getADMaterialProperty<Real>("rho_name")),
    _tau(getADMaterialProperty<Real>("tau")),
    _momentum_strong_residual(getADMaterialProperty<RealVectorValue>("momentum_strong_residual"))
{
}

ADReal
INSADMassPSPGBC::computeQpResidual()
{
  return _tau[_qp] / _rho[_qp] * _momentum_strong_residual[_qp] * _test[_i][_qp] * _normals[_qp];
}
