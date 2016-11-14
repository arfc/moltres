#include "PrecursorKernelAction.h"
#include "Factory.h"
#include "Parser.h"
#include "Conversion.h"
#include "FEProblem.h"

template<>
InputParameters validParams<PrecursorKernelAction>()
{
  InputParameters params = validParams<AddVariableAction>();
  params.addRequiredParam<int>("num_precursor_groups", "specifies the total number of precursors to create");
  params.addRequiredParam<std::string>("var_name_base", "specifies the base name of the variables");
  params.addParam<VariableName>("temperature", "Name of temperature variable");
  params.addParam<VariableName>("u", "Name of x-component of velocity");
  params.addParam<VariableName>("v", "Name of y-component of velocity");
  params.addParam<VariableName>("w", "Name of z-component of velocity");
  params.addParam<Real>("u_def", "Allows user to specify constant value for u component of velocity.");
  params.addParam<Real>("v_def", "Allows user to specify constant value for v component of velocity.");
  params.addParam<Real>("w_def", "Allows user to specify constant value for w component of velocity.");
  params.addRequiredParam<std::vector<VariableName> >("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addRequiredParam<int>("num_groups", "The total number of energy groups.");
  params.addParam<bool>("transient_simulation", false, "Whether to conduct a transient simulation.");
  params.addRequiredParam<std::vector<BoundaryName> >("inlet_boundary", "The inlet boundary for the precursors.");
  params.addRequiredParam<std::string>("inlet_boundary_condition", "The type of boundary condition to apply at the inlet.");
  params.addParam<Real>("inlet_bc_value", "For value based boundary conditions, specify the value.");
  params.addRequiredParam<std::vector<BoundaryName> >("outlet_boundary", "The outlet boundary for the precursors.");
  params.addParam<bool>("add_artificial_diffusion", true, "Whether to add artificial diffusion");
  params.addParam<bool>("incompressible_flow", true, "Determines whether we use a divergence-free form of the advecting velocity.");
  params.addParam<bool>("use_exp_form", "Whether concentrations should be in an expotential/logarithmic format.");
  params.addParam<bool>("jac_test", false, "Whether we're testing the Jacobian and should use some random initial conditions for the precursors.");
  return params;
}

PrecursorKernelAction::PrecursorKernelAction(const InputParameters & params) :
    AddVariableAction(params),
    _num_precursor_groups(getParam<int>("num_precursor_groups")),
    _var_name_base(getParam<std::string>("var_name_base")),
    _num_groups(getParam<int>("num_groups"))
{
}

void
PrecursorKernelAction::act()
{
  for (int op = 1; op <= _num_precursor_groups; ++op)
  {
    //
    // Create variable names
    //

    std::string var_name = _var_name_base + Moose::stringify(op);

    if (_current_task == "add_variable")
      addVariable(var_name);

    if (_current_task == "add_kernel")
    {
      //
      // Set up advection kernels
      //
      if (getParam<bool>("incompressible_flow"))
      {
        InputParameters params = _factory.getValidParams("DivFreeCoupledScalarAdvection");
        params.set<NonlinearVariableName>("variable") = var_name;
        if (isParamValid("u"))
          params.set<std::vector<VariableName> >("u") = {getParam<VariableName>("u")};
        else if (isParamValid("u_def"))
          params.set<Real>("u_def") = getParam<Real>("u_def");
        if (isParamValid("v"))
          params.set<std::vector<VariableName> >("v") = {getParam<VariableName>("v")};
        else if (isParamValid("v_def"))
          params.set<Real>("v_def") = getParam<Real>("v_def");
        if (isParamValid("w"))
          params.set<std::vector<VariableName> >("w") = {getParam<VariableName>("w")};
        else if (isParamValid("w_def"))
          params.set<Real>("w_def") = getParam<Real>("w_def");
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");

        std::string kernel_name = "DivFreeCoupledScalarAdvection_" + var_name;
        _problem->addKernel("DivFreeCoupledScalarAdvection", kernel_name, params);
      }
      else
      {
        InputParameters params = _factory.getValidParams("CoupledScalarAdvection");
        params.set<NonlinearVariableName>("variable") = var_name;
        if (isParamValid("u"))
          params.set<std::vector<VariableName> >("u") = {getParam<VariableName>("u")};
        else if (isParamValid("u_def"))
          params.set<Real>("u_def") = getParam<Real>("u_def");
        if (isParamValid("v"))
          params.set<std::vector<VariableName> >("v") = {getParam<VariableName>("v")};
        else if (isParamValid("v_def"))
          params.set<Real>("v_def") = getParam<Real>("v_def");
        if (isParamValid("w"))
          params.set<std::vector<VariableName> >("w") = {getParam<VariableName>("w")};
        else if (isParamValid("w_def"))
          params.set<Real>("w_def") = getParam<Real>("w_def");
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
        if (isParamValid("use_exp_form"))
          params.set<bool>("use_exp_form") = getParam<bool>("use_exp_form");

        std::string kernel_name = "CoupledScalarAdvection_" + var_name;
        _problem->addKernel("CoupledScalarAdvection", kernel_name, params);
      }


      // Set up PrecursorSource kernels

      {
        InputParameters params = _factory.getValidParams("PrecursorSource");
        params.set<NonlinearVariableName>("variable") = var_name;
        params.set<int>("num_groups") = _num_groups;
        params.set<std::vector<VariableName> >("group_fluxes") = getParam<std::vector<VariableName> >("group_fluxes");
        params.set<int>("precursor_group_number") = op;
        if (isParamValid("temperature"))
          params.set<std::vector<VariableName> >("temperature") = {getParam<VariableName>("temperature")};
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");

        std::string kernel_name = "PrecursorSource_" + var_name;
        _problem->addKernel("PrecursorSource", kernel_name, params);
      }

      //
      // Set up PrecursorDecay kernels
      //

      {
        InputParameters params = _factory.getValidParams("PrecursorDecay");
        params.set<NonlinearVariableName>("variable") = var_name;
        params.set<int>("precursor_group_number") = op;
        if (isParamValid("temperature"))
          params.set<std::vector<VariableName> >("temperature") = {getParam<VariableName>("temperature")};
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");

        std::string kernel_name = "PrecursorDecay_" + var_name;
        _problem->addKernel("PrecursorDecay", kernel_name, params);
      }

      //
      // If doing a transient simulation, set up TimeDerivative kernels
      //

      if (getParam<bool>("transient_simulation"))
      {
        InputParameters params = _factory.getValidParams("ScalarTransportTimeDerivative");
        params.set<NonlinearVariableName>("variable") = var_name;
        params.set<bool>("implicit") = true;
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");

        std::string kernel_name = "ScalarTransportTimeDerivative_" + var_name;
        _problem->addKernel("ScalarTransportTimeDerivative", kernel_name, params);
      }

      // Set up artificial diffusion

      if (getParam<bool>("add_artificial_diffusion"))
      {
        InputParameters params = _factory.getValidParams("ScalarAdvectionArtDiff");
        params.set<NonlinearVariableName>("variable") = var_name;
        if (isParamValid("u"))
          params.set<std::vector<VariableName> >("u") = {getParam<VariableName>("u")};
        else if (isParamValid("u_def"))
          params.set<Real>("u_def") = getParam<Real>("u_def");
        if (isParamValid("v"))
          params.set<std::vector<VariableName> >("v") = {getParam<VariableName>("v")};
        else if (isParamValid("v_def"))
          params.set<Real>("v_def") = getParam<Real>("v_def");
        if (isParamValid("w"))
          params.set<std::vector<VariableName> >("w") = {getParam<VariableName>("w")};
        else if (isParamValid("w_def"))
          params.set<Real>("w_def") = getParam<Real>("w_def");
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");

        std::string kernel_name = "ScalarAdvectionArtDiff_" + var_name;
        _problem->addKernel("ScalarAdvectionArtDiff", kernel_name, params);
      }
    }

    if (_current_task == "add_bc")
    {

      // Set up precursor inlet boundary conditions


      {
        std::string bc_type_name = getParam<std::string>("inlet_boundary_condition");
        InputParameters params = _factory.getValidParams(bc_type_name);
        params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("inlet_boundary");
        params.set<NonlinearVariableName>("variable") = var_name;
        if (isParamValid("inlet_bc_value"))
          params.set<Real>("value") = getParam<Real>("inlet_bc_value");
        std::string bc_name = bc_type_name + "_" + var_name;
        _problem->addBoundaryCondition(bc_type_name, bc_name, params);
      }

      //
      // Set up precursor outlet boundary conditions
      //

      if (!(getParam<bool>("incompressible_flow")))
      {
        InputParameters params = _factory.getValidParams("CoupledScalarAdvectionNoBCBC");
        params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("outlet_boundary");
        params.set<NonlinearVariableName>("variable") = var_name;
        if (isParamValid("u"))
          params.set<std::vector<VariableName> >("u") = {getParam<VariableName>("u")};
        else if (isParamValid("u_def"))
          params.set<Real>("u_def") = getParam<Real>("u_def");
        if (isParamValid("v"))
          params.set<std::vector<VariableName> >("v") = {getParam<VariableName>("v")};
        else if (isParamValid("v_def"))
          params.set<Real>("v_def") = getParam<Real>("v_def");
        if (isParamValid("w"))
          params.set<std::vector<VariableName> >("w") = {getParam<VariableName>("w")};
        else if (isParamValid("w_def"))
          params.set<Real>("w_def") = getParam<Real>("w_def");

        std::string bc_name = "CoupledScalarAdvectionNoBCBC_" + var_name;
        _problem->addBoundaryCondition("CoupledScalarAdvectionNoBCBC", bc_name, params);
      }

      // Set up artificial diffusion

      if (getParam<bool>("add_artificial_diffusion"))
      {
        InputParameters params = _factory.getValidParams("ScalarAdvectionArtDiffNoBCBC");
        params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("outlet_boundary");
        params.set<NonlinearVariableName>("variable") = var_name;
        if (isParamValid("u"))
          params.set<std::vector<VariableName> >("u") = {getParam<VariableName>("u")};
        else if (isParamValid("u_def"))
          params.set<Real>("u_def") = getParam<Real>("u_def");
        if (isParamValid("v"))
          params.set<std::vector<VariableName> >("v") = {getParam<VariableName>("v")};
        else if (isParamValid("v_def"))
          params.set<Real>("v_def") = getParam<Real>("v_def");
        if (isParamValid("w"))
          params.set<std::vector<VariableName> >("w") = {getParam<VariableName>("w")};
        else if (isParamValid("w_def"))
          params.set<Real>("w_def") = getParam<Real>("w_def");

        std::string bc_name = "ScalarAdvectionArtDiffNoBCBC_" + var_name;
        _problem->addBoundaryCondition("ScalarAdvectionArtDiffNoBCBC", bc_name, params);
      }
    }

    // Set up ICs

    if (_current_task == "add_ic")
    {
      if (getParam<bool>("jac_test"))
      {
        InputParameters params = _factory.getValidParams("RandomIC");
        params.set<VariableName>("variable") = var_name;
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
        params.set<Real>("min") = 0;
        params.set<Real>("max") = 1;

        std::string ic_name = "RandomIC_" + var_name;
        _problem->addInitialCondition("RandomIC", ic_name, params);
      }

      else if (isParamValid("initial_condition"))
      {
        InputParameters params = _factory.getValidParams("ConstantIC");
        params.set<VariableName>("variable") = var_name;
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
        params.set<Real>("value") = getParam<Real>("initial_condition");

        std::string ic_name = "ConstantIC_" + var_name;
        _problem->addInitialCondition("ConstantIC", ic_name, params);
      }
    }


    if (getParam<bool>("use_exp_form"))
    {

      std::string aux_var_name = var_name + "_lin";

      // Set up elemental aux variables

      if (_current_task == "add_elemental_field_variable")
      {
        std::set<SubdomainID> blocks = getSubdomainIDs();
        FEType fe_type(CONSTANT, MONOMIAL);
        if (blocks.empty())
          _problem->addAuxVariable(aux_var_name, fe_type);
        else
          _problem->addAuxVariable(aux_var_name, fe_type, &blocks);
      }

      // Set up aux kernels

      if (_current_task == "add_aux_kernel")
      {
        InputParameters params = _factory.getValidParams("Density");
        params.set<AuxVariableName>("variable") = aux_var_name;
        params.set<std::vector<VariableName> >("density_log") = {var_name};
        if (isParamValid("block"))
          params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");

        std::string aux_kernel_name = "Density_" + aux_var_name;
        _problem->addAuxKernel("Density", aux_kernel_name, params);
      }
    }
  }
}
