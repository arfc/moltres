#include "SNDiffusionBC.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNDiffusionBC);

InputParameters
SNDiffusionBC::validParams()
{
  InputParameters params = ArrayIntegratedBC::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number",
                                        "The group for which this bc acts on");
  params.addRequiredCoupledVar("diffusion_flux", "Neutron diffusion flux variable");
  return params;
}

SNDiffusionBC::SNDiffusionBC(const InputParameters & parameters)
  : ArrayIntegratedBC(parameters),
    _N(getParam<unsigned int>("N")),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _group(getParam<unsigned int>("group_number") - 1),
    _diff_flux(coupledValue("diffusion_flux")),
    _grad_diff_flux(coupledGradient("diffusion_flux"))
{
  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNDiffusionBC::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector normal_vec{{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)}};
  RealEigenVector ord_dot_n = _ordinates * normal_vec;
  residual = _test[_i][_qp] * ord_dot_n.cwiseProduct(_u[_qp]).cwiseProduct(_weights);
  RealEigenVector grad_flux_vec{{_grad_diff_flux[_qp](0),
                                 _grad_diff_flux[_qp](1),
                                 _grad_diff_flux[_qp](2)}};
  for (unsigned int i = 0; i < _count; ++i)
    if (ord_dot_n(i) < 0.)
      residual(i) = _test[_i][_qp] * _weights(i) * ord_dot_n(i) * (_diff_flux[_qp] -
          3. * _diffcoef[_qp][_group] * _ordinates.row(i) * grad_flux_vec) * 0.125;
}

RealEigenVector
SNDiffusionBC::computeQpJacobian()
{
  RealEigenVector normal_vec{{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)}};
  RealEigenVector ord_dot_n = _ordinates * normal_vec;
  RealEigenVector jac;
  jac = _test[_i][_qp] * _phi[_j][_qp] * ord_dot_n.cwiseProduct(_weights);
  for (unsigned int i = 0; i < _count; ++i)
    if (ord_dot_n(i) < 0.)
      jac(i) = 0.;
  return jac;
}
