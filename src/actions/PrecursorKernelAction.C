#include "PrecursorKernelAction.h"
#include "Factory.h"
#include "Parser.h"
#include "Conversion.h"
#include "FEProblem.h"

template<>
InputParameters validParams<PrecursorKernelAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<int>("num_precursor_groups", "specifies the total number of precursors to create");
  params.addRequiredParam<std::string>("var_name_base", "specifies the base name of the variables");
  params.addParam<VariableName>("c", "Name of coupled concentration variable");
  params.addParam<VariableName>("T", "Name of temperature variable");
  params.addParam<VariableName>("u", "Name of x-component of velocity");
  params.addParam<VariableName>("v", "Name of y-component of velocity");
  params.addParam<VariableName>("w", "Name of z-component of velocity");
  params.addRequiredParam<VariableName>("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addRequiredParam<int>("num_groups", "The total number of energy groups.");
  params.addParam<bool>("transient_simulation", false, "Whether to conduct a transient simulation.");
  return params;
}

PrecursorKernelAction::PrecursorKernelAction(const InputParameters & params) :
    Action(params),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _var_name_base(getParam<std::string>("var_name_base")),
    _num_groups(getParam<int>("num_groups"))
{
}

void
PrecursorKernelAction::act()
{
  for (unsigned int op = 1; op <= _num_precursor_groups; ++op)
  {
    //
    // Create variable names
    //

    std::string var_name = _var_name_base + Moose::stringify(op);

    //
    // Set up advection kernels
    //

    {
      InputParameters params = _factory.getValidParams("CoupledScalarAdvection");
      params.set<NonlinearVariableName>("variable") = var_name;
      if (isParamValid("u"))
        params.set<std::vector<VariableName> >("u") = {getParam<VariableName>("u")};
      if (isParamValid("v"))
        params.set<std::vector<VariableName> >("v") = {getParam<VariableName>("v")};
      if (isParamValid("w"))
        params.set<std::vector<VariableName> >("w") = {getParam<VariableName>("w")};

      std::string kernel_name = "CoupledScalarAdvection_" + var_name;
      _problem->addKernel("CoupledScalarAdvection", kernel_name, params);
    }

    //
    // Set up PrecursorSource kernels
    //

    {
      InputParameters params = _factory.getValidParams("PrecursorSource");
      params.set<NonlinearVariableName>("variable") = var_name;
      params.set<int>("num_groups") = _num_groups;
      params.set<std::vector<VariableName> >("group_fluxes") = {getParam<VariableName>("group_fluxes")};
      params.set<int>("precursor_group_number") = op - 1;
      if (isParamValid("T"))
        params.set<std::vector<VariableName> >("temperature") = {getParam<VariableName>("T")};

      std::string kernel_name = "PrecursorSource_" + var_name;
      _problem->addKernel("PrecursorSource", kernel_name, params);
    }

    //
    // Set up PrecursorDecay kernels
    //

    {
      InputParameters params = _factory.getValidParams("PrecursorDecay");
      params.set<NonlinearVariableName>("variable") = var_name;
      params.set<int>("precursor_group_number") = op - 1;
      if (isParamValid("T"))
        params.set<std::vector<VariableName> >("temperature") = {getParam<VariableName>("T")};

      std::string kernel_name = "PrecursorDecay_" + var_name;
      _problem->addKernel("PrecursorDecay", kernel_name, params);
    }

    //
    // If doing a transient simulation, set up TimeDerivative kernels
    //

    if (getParam<bool>("transient_simulation"))
    {
      InputParameters params = _factory.getValidParams("TimeDerivative");
      params.set<NonlinearVariableName>("variable") = var_name;
      params.set<bool>("implicit") = true;
      std::string kernel_name = "TimeDerivative_" + var_name;
      _problem->addKernel("TimeDerivative", kernel_name, params);
    }
  }
}
