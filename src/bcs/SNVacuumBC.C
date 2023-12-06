#include "SNVacuumBC.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNVacuumBC);

InputParameters
SNVacuumBC::validParams()
{
  InputParameters params = ArrayIntegratedBC::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  return params;
}

SNVacuumBC::SNVacuumBC(const InputParameters & parameters)
  : ArrayIntegratedBC(parameters),
    _N(getParam<unsigned int>("N"))
{
  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNVacuumBC::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector normal_vec{{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)}};
  RealEigenVector ord_dot_n = _ordinates * normal_vec;
  residual = _test[_i][_qp] * ord_dot_n.cwiseProduct(_u[_qp]).cwiseProduct(_weights);
  for (unsigned int i = 0; i < _count; ++i)
    if (ord_dot_n(i) < 0.)
      residual(i) = 0.;
}

RealEigenVector
SNVacuumBC::computeQpJacobian()
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
