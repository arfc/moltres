#include "GroupDiffusion.h"

registerMooseObject("MoltresApp", GroupDiffusion);

InputParameters
GroupDiffusion::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("group_number",
                                        "The group for which this kernel controls diffusion");
  params.addCoupledVar("temperature",
                       "The temperature used to interpolate the diffusion coefficient");
  params.addParam<bool>("set_diffcoef_limit",
      false,
      "Replaces all diffusion coefficient values above 5.0 to 5.0. "
      "Primarily helps with stabilizing drift coefficients in void regions.");
  return params;
}

GroupDiffusion::GroupDiffusion(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _d_diffcoef_d_temp(getMaterialProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _group(getParam<unsigned int>("group_number") - 1),
    _temp_id(coupled("temperature")),
    _limit(getParam<bool>("set_diffcoef_limit"))
{
}

Real
GroupDiffusion::computeQpResidual()
{
  Real diffcoef;
  if (_limit && _diffcoef[_qp][_group] > 5.0)
    diffcoef = 5.0;
  else
    diffcoef = _diffcoef[_qp][_group];
  return diffcoef * _grad_test[_i][_qp] *
         computeConcentrationGradient(_u, _grad_u, _qp);
}

Real
GroupDiffusion::computeQpJacobian()
{
  Real diffcoef;
  if (_limit && _diffcoef[_qp][_group] > 5.0)
    diffcoef = 5.0;
  else
    diffcoef = _diffcoef[_qp][_group];
  return diffcoef * _grad_test[_i][_qp] *
         computeConcentrationGradientDerivative(_u, _grad_u, _phi, _grad_phi, _j, _qp);
}

Real
GroupDiffusion::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_limit && _diffcoef[_qp][_group] > 5.0)
    return 0;
  if (jvar == _temp_id)
    return _d_diffcoef_d_temp[_qp][_group] * _phi[_j][_qp] * _grad_test[_i][_qp] *
           computeConcentrationGradient(_u, _grad_u, _qp);
  else
    return 0;
}
