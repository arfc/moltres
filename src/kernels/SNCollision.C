#include "SNCollision.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNCollision);

InputParameters
SNCollision::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNCollision::SNCollision(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _totxs(getMaterialProperty<std::vector<Real>>("totxs")),
    _d_totxs_d_temp(getMaterialProperty<std::vector<Real>>("d_totxs_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
  // Level-symmetric quadrature weights
  _weights = MoltresUtils::level_symmetric(_N).col(3);
}

void
SNCollision::computeQpResidual(RealEigenVector & residual)
{
  residual = _test[_i][_qp] * _totxs[_qp][_group] * _weights.cwiseProduct(_u[_qp]);
}

RealEigenVector
SNCollision::computeQpJacobian()
{
  return _test[_i][_qp] * _totxs[_qp][_group] * _phi[_j][_qp] * _weights;
}

RealEigenMatrix
SNCollision::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (jvar.number() == _temp_id)
    return _test[_i][_qp] * _d_totxs_d_temp[_qp][_group] * _phi[_j][_qp] *
      _weights.cwiseProduct(_u[_qp]);
  else
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
