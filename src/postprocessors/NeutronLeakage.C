#include "NeutronLeakage.h"

registerMooseObject("MoltresApp", NeutronLeakage);

InputParameters
NeutronLeakage::validParams()
{
  InputParameters params = SideIntegralVariablePostprocessor::validParams();
  params.addClassDescription("Postprocessor for computing neutron leakage along provided "
                             "boundaries");
  params.addRequiredParam<unsigned int>("group_number",
                                        "The group for which this postprocessor "
                                        "calculates leakage");
  params.addCoupledVar("temperature",
                       "The temperature used to interpolate the diffusion coefficient");
  return params;
}

NeutronLeakage::NeutronLeakage(const InputParameters & parameters)
  : SideIntegralVariablePostprocessor(parameters),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _group(getParam<unsigned int>("group_number") - 1)
{
  addMooseVariableDependency(&mooseVariableField());
}

Real
NeutronLeakage::computeQpIntegral()
{
  return _u[_qp] / 4 - _normals[_qp] * _grad_u[_qp] * _diffcoef[_qp][_group] / 2;
}
