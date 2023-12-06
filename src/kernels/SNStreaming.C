#include "SNStreaming.h"

#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNStreaming);

InputParameters
SNStreaming::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNStreaming::SNStreaming(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _totxs(getMaterialProperty<std::vector<Real>>("totxs")),
    _d_totxs_d_temp(getMaterialProperty<std::vector<Real>>("d_totxs_d_temp")),
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
SNStreaming::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector lhs = _ordinates * _array_grad_test[_i][_qp];
  RealEigenVector rhs =
    (_tau_sn[_qp][_group] * _ordinates.cwiseProduct(_grad_u[_qp]).rowwise().sum() -
     ((1. - _tau_sn[_qp][_group] * _totxs[_qp][_group]) * _u[_qp]));
  residual = _weights.cwiseProduct(lhs).cwiseProduct(rhs);
}

RealEigenVector
SNStreaming::computeQpJacobian()
{
  RealEigenVector array_phi = RealEigenVector::Constant(_var.count(), _phi[_j][_qp]);
  RealEigenVector array_grad_phi(3);
  array_grad_phi << _grad_phi[_j][_qp](0),
                    _grad_phi[_j][_qp](1),
                    _grad_phi[_j][_qp](2);
  RealEigenVector lhs = _ordinates * _array_grad_test[_i][_qp];
  RealEigenVector rhs = (_tau_sn[_qp][_group] * (_ordinates * array_grad_phi) -
                         ((1. - _tau_sn[_qp][_group] * _totxs[_qp][_group]) * array_phi));
  return _weights.cwiseProduct(lhs).cwiseProduct(rhs);
}

RealEigenMatrix
SNStreaming::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (jvar.number() == _temp_id)
  {
    RealEigenVector lhs = _ordinates * _array_grad_test[_i][_qp];
    RealEigenVector rhs =
      (_tau_sn[_qp][_group] * _d_totxs_d_temp[_qp][_group] * _phi[_j][_qp] * _u[_qp]);
    return _weights.cwiseProduct(lhs).cwiseProduct(rhs);
  }
  else
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
