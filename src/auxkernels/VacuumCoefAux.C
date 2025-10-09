#include "VacuumCoefAux.h"

#include "Assembly.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", VacuumCoefAux);

InputParameters
VacuumCoefAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredCoupledVar("psi", "SN angular flux array variable");
  return params;
}

VacuumCoefAux::VacuumCoefAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _psi(coupledArrayValue("psi")),
    _normals(_assembly.normals())
{
  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(getParam<unsigned int>("N"));
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

Real
VacuumCoefAux::computeValue()
{
  Real value = 0.0;
  RealEigenVector normal_vec{{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)}};
  RealEigenVector ord_dot_n = _ordinates * normal_vec;
  RealEigenVector w_psi = _weights.cwiseProduct(_psi[_qp]);
  for (unsigned int i = 0; i < ord_dot_n.size(); ++i)
    if (ord_dot_n(i) > 0.0)
      value += w_psi(i) * ord_dot_n(i);
  Real denom = w_psi.sum();
  return value / denom - 0.5;
}
