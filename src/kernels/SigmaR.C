#include "SigmaR.h"

template<>
InputParameters validParams<SigmaR>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<int>("group_number", "The current energy group.");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}


SigmaR::SigmaR(const InputParameters & parameters) :
    Kernel(parameters),
    ScalarTransportBase(parameters),
    _remxs(getMaterialProperty<std::vector<Real> >("remxs")),
    _d_remxs_d_temp(getMaterialProperty<std::vector<Real> >("d_remxs_d_temp")),
    _group(getParam<int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
}

Real
SigmaR::computeQpResidual()
{
  return _test[_i][_qp] * _remxs[_qp][_group] * computeConcentration(_u, _qp);
}

Real
SigmaR::computeQpJacobian()
{
  return _test[_i][_qp] * _remxs[_qp][_group] * computeConcentrationDerivative(_u, _phi, _j, _qp);
}

Real
SigmaR::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_id)
    return _test[_i][_qp] * _d_remxs_d_temp[_qp][_group] * _phi[_j][_qp] * computeConcentration(_u, _qp);
  else
    return 0;
}
