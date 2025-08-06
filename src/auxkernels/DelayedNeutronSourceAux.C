#include "DelayedNeutronSourceAux.h"

registerMooseObject("MoltresApp", DelayedNeutronSourceAux);

InputParameters
DelayedNeutronSourceAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("num_precursor_groups", "The number of precursor groups.");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("pre_concs", "All the variables that hold the precursor "
                                            "concentrations. These MUST be listed by increasing "
                                            "group number.");
  return params;
}

DelayedNeutronSourceAux::DelayedNeutronSourceAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    ScalarTransportBase(parameters),
    _decay_constant(getMaterialProperty<std::vector<Real>>("decay_constant")),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _temp(coupledValue("temperature"))
{
  unsigned int n = coupledComponents("pre_concs");
  if (!(n == _num_precursor_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _pre_concs.resize(n);
  for (unsigned int i = 0; i < _pre_concs.size(); ++i)
  {
    _pre_concs[i] = &coupledValue("pre_concs", i);
  }
}

Real
DelayedNeutronSourceAux::computeValue()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_precursor_groups; ++i)
    r += _decay_constant[_qp][i] * computeConcentration((*_pre_concs[i]), _qp);

  return r;
}
