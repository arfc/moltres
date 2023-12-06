#include "SNTimeDerivative.h"

registerMooseObject("MoltresApp", SNTimeDerivative);

InputParameters
SNTimeDerivative::validParams()
{
  InputParameters params = ArrayTimeKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNTimeDerivative::SNTimeDerivative(const InputParameters & parameters)
  : ArrayTimeKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _recipvel(getMaterialProperty<std::vector<Real>>("recipvel")),
    _d_recipvel_d_temp(getMaterialProperty<std::vector<Real>>("d_recipvel_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNTimeDerivative::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);
  residual = _recipvel[_qp][_group] * _weights.cwiseProduct(lhs).cwiseProduct(_u_dot[_qp]);
}

RealEigenVector
SNTimeDerivative::computeQpJacobian()
{
  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);
  return _recipvel[_qp][_group] * _phi[_j][_qp] * _du_dot_du[_qp] * _weights.cwiseProduct(lhs);
}

RealEigenMatrix
SNTimeDerivative::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (jvar.number() == _temp_id)
  {
    RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
      RealEigenVector::Constant(_count, _test[_i][_qp]);
    return _d_recipvel_d_temp[_qp][_group] * _phi[_j][_qp] * _du_dot_du[_qp] *
      _weights.cwiseProduct(lhs);
  }
  else
    return ArrayTimeKernel::computeQpOffDiagJacobian(jvar);
}
