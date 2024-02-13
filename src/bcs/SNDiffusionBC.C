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
  params.addCoupledVar("temperature",
                       "The temperature used to interpolate the diffusion coefficient");
  return params;
}

SNDiffusionBC::SNDiffusionBC(const InputParameters & parameters)
  : ArrayIntegratedBC(parameters),
    _N(getParam<unsigned int>("N")),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _d_diffcoef_d_temp(getMaterialProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _group(getParam<unsigned int>("group_number") - 1),
    _diff_flux(coupledValue("diffusion_flux")),
    _grad_diff_flux(coupledGradient("diffusion_flux")),
    _temp_id(coupled("temperature"))
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
  Real sum = 0.;
  for (unsigned int i = 0; i < _count; ++i)
    if (ord_dot_n(i) < 0.)
      sum += ord_dot_n(i) * _weights(i);
  sum /= 8.;
  for (unsigned int i = 0; i < _count; ++i)
    if (ord_dot_n(i) < 0.)
      residual(i) = (_diff_flux[_qp] / 4. + _diffcoef[_qp][_group] / 2. * (_grad_diff_flux[_qp] *
          _normals[_qp])) / sum;
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
