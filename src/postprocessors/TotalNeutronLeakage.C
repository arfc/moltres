#include "TotalNeutronLeakage.h"

registerMooseObject("MoltresApp", TotalNeutronLeakage);

InputParameters
TotalNeutronLeakage::validParams()
{
  InputParameters params = SideIntegralPostprocessor::validParams();
  params += ScalarTransportBase::validParams();
  params.addClassDescription("Postprocessor for computing total neutron leakage along provided "
                             "boundaries for all neutron group fluxes");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  return params;
}

TotalNeutronLeakage::TotalNeutronLeakage(const InputParameters & parameters)
  : SideIntegralPostprocessor(parameters),
    ScalarTransportBase(parameters),
    _vars(getCoupledMooseVars()),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _num_groups(getParam<unsigned int>("num_groups"))
{
  addMooseVariableDependency(_vars);
  unsigned int n = coupledComponents("group_fluxes");
  if (n != _num_groups)
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _group_fluxes.resize(n);
  _grad_group_fluxes.resize(n);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _grad_group_fluxes[i] = &coupledGradient("group_fluxes", i);
  }
}

Real
TotalNeutronLeakage::computeQpIntegral()
{
  Real value = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    value += computeConcentration((*_group_fluxes[i]), _qp) / 4 - _normals[_qp] *
             computeConcentrationGradient((*_group_fluxes[i]), (*_grad_group_fluxes[i]), _qp) *
             _diffcoef[_qp][i] / 2;
  }
  return value;
}
