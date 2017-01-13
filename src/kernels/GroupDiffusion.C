#include "GroupDiffusion.h"

template<>
InputParameters validParams<GroupDiffusion>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<int>("group_number", "The group for which this kernel controls diffusion");
  params.addCoupledVar("temperature", 800, "The temperature used to interpolate the diffusion coefficient");
  return params;
}


GroupDiffusion::GroupDiffusion(const InputParameters & parameters) :
    Kernel(parameters),
    ScalarTransportBase(parameters),
    _diffcoef(getMaterialProperty<std::vector<Real> >("diffcoef")),
    _d_diffcoef_d_temp(getMaterialProperty<std::vector<Real> >("d_diffcoef_d_temp")),
    _group(getParam<int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
}

Real
GroupDiffusion::computeQpResidual()
{
  return _diffcoef[_qp][_group] * _grad_test[_i][_qp] * computeConcentrationGradient(_u, _grad_u, _qp);
}

Real
GroupDiffusion::computeQpJacobian()
{
  return _diffcoef[_qp][_group] * _grad_test[_i][_qp] * computeConcentrationGradientDerivative(_u, _grad_u, _phi, _grad_phi, _j, _qp);
}

Real
GroupDiffusion::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_id)
    return _d_diffcoef_d_temp[_qp][_group] * _phi[_j][_qp] * _grad_test[_i][_qp] * computeConcentrationGradient(_u, _grad_u, _qp);
  else
    return 0;
}
