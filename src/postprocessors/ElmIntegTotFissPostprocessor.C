#include "ElmIntegTotFissPostprocessor.h"

template<>
InputParameters validParams<ElmIntegTotFissPostprocessor>()
{
  InputParameters params = validParams<ElementIntegralPostprocessor>();
  params.addRequiredCoupledVar("group_fluxes", "The group fluxes. MUST be arranged by decreasing energy/increasing group number.");
  params.addRequiredParam<int>("num_groups", "The number of energy groups.");
  return params;
}

ElmIntegTotFissPostprocessor::ElmIntegTotFissPostprocessor(const InputParameters & parameters) :
    ElementIntegralPostprocessor(parameters),
    // MooseVariableInterface(this, false),
    _num_groups(getParam<int>("num_groups")),
    _fissxs(getMaterialProperty<std::vector<Real> >("fissxs")),
    _vars(getCoupledMooseVars())
{
  addMooseVariableDependency(_vars);
  int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
    mooseError("The number of coupled variables doesn't match the number of groups.");

  _group_fluxes.resize(n);
  for (int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
  }
}

Real
ElmIntegTotFissPostprocessor::computeQpIntegral()
{
  Real sum = 0;
  for (int i = 0; i < _num_groups; ++i)
    sum += _fissxs[_qp][i] * (*_group_fluxes[i])[_qp];

  return sum;
}
