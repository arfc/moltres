#include "SNExternalSource.h"
#include "Function.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNExternalSource);

InputParameters
SNExternalSource::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<std::vector<FunctionName>>("function", "The external source functions");
  return params;
}

SNExternalSource::SNExternalSource(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1)
{
  auto & funcs = getParam<std::vector<FunctionName>>("function");
  if (_var.count() != funcs.size())
    paramError("function",
               "Number of external source functions must agree with the number of angular flux "
               "array variable components");
  for (auto & func : funcs)
    _func.push_back(&getFunctionByName(func));

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNExternalSource::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp];
  for (unsigned int i = 0; i < _count; ++i)
    residual(i) = -_weights(i) * (lhs(i) + _test[_i][_qp]) * _func[i]->value(_t, _q_point[_qp]);
}
