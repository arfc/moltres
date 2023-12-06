#include "SNReflectingBC.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNReflectingBC);

InputParameters
SNReflectingBC::validParams()
{
  InputParameters params = ArrayIntegratedBC::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  return params;
}

SNReflectingBC::SNReflectingBC(const InputParameters & parameters)
  : ArrayIntegratedBC(parameters),
    _N(getParam<unsigned int>("N"))
{
  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNReflectingBC::computeQpResidual(RealEigenVector & residual)
{
  if (std::abs(_normals[_qp](0) + _normals[_qp](1) + _normals[_qp](2)) < 1.0-1e-6)
    mooseError("Reflecting boundaries for angular fluxes can only be applied to plane boundaries "
               "perpendicular to the x-, y-, or z-axis.");
  RealEigenVector normal_vec{{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)}};
  RealEigenVector ord_dot_n = _ordinates * normal_vec;
  RealEigenVector flux = _u[_qp];
  for (unsigned int i = 0; i < _count; ++i)
    if (ord_dot_n(i) < 0.)
    {
      if (std::abs(_normals[_qp](0)) > 1.0-1e-6)
        flux(i) = _u[_qp](MoltresUtils::x_reflection_map(i));
      else if (std::abs(_normals[_qp](1)) > 1.0-1e-6)
        flux(i) = _u[_qp](MoltresUtils::y_reflection_map(i));
      else
        flux(i) = _u[_qp](MoltresUtils::z_reflection_map(i));
    }
  residual = _test[_i][_qp] * ord_dot_n.cwiseProduct(flux).cwiseProduct(_weights);
}

RealEigenVector
SNReflectingBC::computeQpJacobian()
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

RealEigenMatrix
SNReflectingBC::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (jvar.number() == _var.number())
  {
    RealEigenMatrix jac = RealEigenMatrix::Zero(_count, _count);
    RealEigenVector normal_vec{{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)}};
    RealEigenVector ord_dot_n = _ordinates * normal_vec;
    for (unsigned int i = 0; i < _count; ++i)
    {
      if (ord_dot_n(i) < 0.)
      {
        if (std::abs(_normals[_qp](0)) > 1.0-1e-6)
          jac(i, MoltresUtils::x_reflection_map(i)) =
            _test[_i][_qp] * _phi[_j][_qp] * ord_dot_n(i) * _weights(i);
        else if (std::abs(_normals[_qp](1)) > 1.0-1e-6)
          jac(i, MoltresUtils::y_reflection_map(i)) =
            _test[_i][_qp] * _phi[_j][_qp] * ord_dot_n(i) * _weights(i);
        else
          jac(i, MoltresUtils::z_reflection_map(i)) =
            _test[_i][_qp] * _phi[_j][_qp] * ord_dot_n(i) * _weights(i);
      }
      else
        jac(i, i) = _test[_i][_qp] * _phi[_j][_qp] * ord_dot_n(i) * _weights(i);
    }
    return jac;
  }
  else
    return ArrayIntegratedBC::computeQpOffDiagJacobian(jvar);
}
