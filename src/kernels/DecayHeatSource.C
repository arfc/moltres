#include "DecayHeatSource.h"

registerMooseObject("MoltresApp", DecayHeatSource);

template <>
InputParameters
validParams<DecayHeatSource>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("num_decay_heat_groups", "The number of decay heat groups.");
  params.addRequiredCoupledVar("heat_concs", "All the variables that hold the decay heat "
                                             "precursor concentrations.");
  params.addRequiredParam<std::vector<Real>>("decay_heat_constants", "Decay Heat Constants");
  return params;
}

DecayHeatSource::DecayHeatSource(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _num_heat_groups(getParam<unsigned int>("num_decay_heat_groups")),
    _decay_heat_const(getParam<std::vector<Real>>("decay_heat_constants"))
{
  unsigned int n = coupledComponents("heat_concs");
  if (!(n == _num_heat_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of decay heat groups.");
  }
  _heat_concs.resize(n);
  _heat_ids.resize(n);
  for (unsigned int i = 0; i < _heat_concs.size(); ++i)
  {
    _heat_concs[i] = &coupledValue("heat_concs", i);
    _heat_ids[i] = coupled("heat_concs", i);
  }
}

Real
DecayHeatSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i=0; i < _num_heat_groups; ++i)
    r += -_test[_i][_qp] * _decay_heat_const[i] * computeConcentration((*_heat_concs[i]), _qp);
  
  return r;
}

Real
DecayHeatSource::computeQpJacobian()
{
  return 0;
}

Real
DecayHeatSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i=0; i < _num_heat_groups; ++i)
  {
    if (jvar == _heat_ids[i])
    {
      jac += -_test[_i][_qp] * _decay_heat_const[i] *
             computeConcentrationDerivative((*_heat_concs[i]), _phi, _j, _qp);
    }
  }
  return jac;
}