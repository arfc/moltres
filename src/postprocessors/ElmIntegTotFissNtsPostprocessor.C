#include "ElmIntegTotFissNtsPostprocessor.h"

registerMooseObject("MoltresApp", ElmIntegTotFissNtsPostprocessor);

InputParameters
ElmIntegTotFissNtsPostprocessor::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  params.addRequiredCoupledVar(
      "group_fluxes",
      "The group fluxes. MUST be arranged by decreasing energy/increasing group number.");
  params.addCoupledVar("pre_concs", "All the variables that hold the precursor "
                                    "concentrations. These MUST be listed by increasing "
                                    "group number.");
  params.addRequiredParam<int>("num_groups", "The number of energy groups.");
  params.addParam<int>("num_precursor_groups", 0, "The number of precursor groups.");
  params.addRequiredParam<bool>("account_delayed", "Whether to account for delayed neutrons.");
  return params;
}

ElmIntegTotFissNtsPostprocessor::ElmIntegTotFissNtsPostprocessor(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    // MooseVariableInterface(this, false),
    _num_groups(getParam<int>("num_groups")),
    _num_precursor_groups(getParam<int>("num_precursor_groups")),
    _account_delayed(getParam<bool>("account_delayed")),
    _nsf(getMaterialProperty<std::vector<Real>>("nsf")),
    _decay_constant(getMaterialProperty<std::vector<Real>>("decay_constant")),
    _vars(getCoupledMooseVars())
{
  addMooseVariableDependency(_vars);
  int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
    mooseError("The number of group flux variables doesn't match the number of energy groups.");

  _group_fluxes.resize(n);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
  }

  if (_account_delayed)
  {
    int m = coupledComponents("pre_concs");
    if (m == 0)
    {
      mooseError("account_delayed flag set to true but no precursor groups specified."
                 "If there are no precursor groups, set account_delayed to false");
    }
    else if (!(m == _num_precursor_groups))
    {
      mooseError("The number of precursor conc variables doesn't match the number"
                 "of precursor groups.");
    }
    _pre_concs.resize(m);
    for (unsigned int i = 0; i < _pre_concs.size(); ++i)
    {
      _pre_concs[i] = &coupledValue("pre_concs", i);
    }
  }
}

Real
ElmIntegTotFissNtsPostprocessor::computeQpIntegral()
{
  Real sum = 0;
  for (int i = 0; i < _num_groups; ++i)
    sum += _nsf[_qp][i] * (*_group_fluxes[i])[_qp];

  if (_account_delayed)
  {
    for (int i = 0; i < _num_precursor_groups; ++i)
      sum += _decay_constant[_qp][i] * (*_pre_concs[i])[_qp];
  }

  return sum;
}
