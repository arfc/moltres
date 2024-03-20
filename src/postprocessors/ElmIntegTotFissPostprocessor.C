#include "ElmIntegTotFissPostprocessor.h"

registerMooseObject("MoltresApp", ElmIntegTotFissPostprocessor);

InputParameters
ElmIntegTotFissPostprocessor::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar(
      "group_fluxes",
      "The group fluxes. MUST be arranged by decreasing energy/increasing group number.");
  params.addRequiredParam<unsigned int>("num_groups", "The number of energy groups.");
  params.addParam<Real>("nt_scale", 1, "Scaling of the neutron fluxes to aid convergence.");
  return params;
}

ElmIntegTotFissPostprocessor::ElmIntegTotFissPostprocessor(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    ScalarTransportBase(parameters),
    _num_groups(getParam<unsigned int>("num_groups")),
    _fissxs(getMaterialProperty<std::vector<Real>>("fissxs")),
    _vars(getCoupledMooseVars()),
    _nt_scale(getParam<Real>("nt_scale"))
{
  addMooseVariableDependency(_vars);
  unsigned int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
    mooseError("The number of coupled variables doesn't match the number of groups.");

  _group_fluxes.resize(n);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
  }
}

Real
ElmIntegTotFissPostprocessor::computeQpIntegral()
{
  Real sum = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
    sum += computeFluxMultiplier(i) * computeConcentration((*_group_fluxes[i]), _qp) * _nt_scale;

  return sum;
}

Real
ElmIntegTotFissPostprocessor::computeFluxMultiplier(int index)
{
  return _fissxs[_qp][index];
}
