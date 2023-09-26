#include "PrecursorAction.h"
#include "Factory.h"
#include "Parser.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "NonlinearSystemBase.h"
#include "DGKernelBase.h"

registerMooseAction("MoltresApp", PrecursorAction, "add_kernel");
registerMooseAction("MoltresApp", PrecursorAction, "add_postprocessor");
registerMooseAction("MoltresApp", PrecursorAction, "add_bc");
registerMooseAction("MoltresApp", PrecursorAction, "add_variable");
registerMooseAction("MoltresApp", PrecursorAction, "add_ic");
registerMooseAction("MoltresApp", PrecursorAction, "add_dg_kernel");
registerMooseAction("MoltresApp", PrecursorAction, "add_transfer");
registerMooseAction("MoltresApp", PrecursorAction, "check_copy_nodal_vars");
registerMooseAction("MoltresApp", PrecursorAction, "copy_nodal_vars");

InputParameters
PrecursorAction::validParams()
{
  InputParameters params = VariableNotAMooseObjectAction::validParams();
  params.addRequiredParam<unsigned int>("num_precursor_groups",
                                        "specifies the total number of precursors to create");
  params.addRequiredParam<std::string>("var_name_base", "specifies the base name of the variables");
  params.addRequiredCoupledVar("temperature", "Name of temperature variable");
  params.addParam<NonlinearVariableName>("uvel", "Name of x-component of velocity");
  params.addParam<NonlinearVariableName>("vvel", "Name of y-component of velocity");
  params.addParam<NonlinearVariableName>("wvel", "Name of z-component of velocity");
  params.addParam<bool>("constant_velocity_values",
                        true,
                        "Whether the velocity components are constant with respect to space");
  params.addParam<Real>("u_def",
                        "Allows user to specify constant value for u component of velocity.");
  params.addParam<Real>("v_def",
                        "Allows user to specify constant value for v component of velocity.");
  params.addParam<Real>("w_def",
                        "Allows user to specify constant value for w component of velocity.");
  params.addParam<FunctionName>(
      "u_func", "Allows user to specify function value for u component of velocity.");
  params.addParam<FunctionName>(
      "v_func", "Allows user to specify function value for v component of velocity.");
  params.addParam<FunctionName>(
      "w_func", "Allows user to specify function value for w component of velocity.");
  params.addRequiredCoupledVar("group_fluxes",
                               "All the variables that hold the group fluxes. "
                               "These MUST be listed by decreasing "
                               "energy/increasing group number.");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups.");
  params.addRequiredParam<std::vector<BoundaryName>>("outlet_boundaries", "Outflow boundaries.");
  params.addParam<std::vector<BoundaryName>>("inlet_boundaries", "Inflow boundaries.");
  params.addParam<bool>("nt_exp_form",
                        false,
                        "Whether concentrations should be in an expotential/logarithmic format.");
  params.addParam<Real>("eigenvalue_scaling",
                        1.0,
                        "Artificial scaling factor for the fission source. Primarily for "
                        "introducing artificial reactivity to make super/subcritical systems "
                        "exactly critical or to simulate reactivity insertions/withdrawals.");
  params.addParam<bool>("jac_test",
                        false,
                        "Whether we're testing the Jacobian and should use some "
                        "random initial conditions for the precursors.");
  params.addParam<bool>("transient", true, "Whether to run a transient simulation.");
  params.addParam<bool>(
      "init_from_file", false, "Whether to initialize the precursors from a file.");
  params.addParam<bool>("create_vars", true, "Whether this action should create the variables.");
  params.addRequiredParam<bool>(
      "loop_precursors", "Whether precursors are circulated in coolant loop.");
  params.addParam<std::string>(
      "object_suffix",
      "",
      "An optional suffix string that can be helpful to avoid object name crashing.");
  params.addParam<std::vector<SubdomainName>>("kernel_block",
                                              "Optional list of block names/IDs in which to"
                                              "initialize kernels if user wishes to omit kernels"
                                              "from the base list of blocks. Replaces the 'block'"
                                              "parameter when initializing kernels.");
  params.addParam<MultiAppName>("multi_app", "Multiapp name for looping precursors.");
  params.addParam<bool>("is_loopapp", false, "if circulating precursors, whether this is loop app");
  params.addParam<bool>("eigen", false, "whether neutronics is in eigenvalue calculation mode");
  params.addParam<NonlinearVariableName>("outlet_vel",
                                         "Name of the velocity variable normal to the "
                                         "outlet for calculating the flow-averaged "
                                         "precursor concentration outflow when using "
                                         "Navier-Stokes flow.");
  return params;
}

PrecursorAction::PrecursorAction(const InputParameters & params)
  : VariableNotAMooseObjectAction(params),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _var_name_base(getParam<std::string>("var_name_base")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _object_suffix(getParam<std::string>("object_suffix")),
    _is_loopapp(getParam<bool>("is_loopapp"))
{
  if (getParam<bool>("loop_precursors"))
  {
    if (!params.isParamSetByUser("inlet_boundaries"))
      mooseError("Looping precursors requires specification of inlet_boundaries.");
    if (!params.isParamValid("multi_app"))
      mooseError("Looping precursors requires a multiapp that governs the loop.");
  }
}

void
PrecursorAction::addRelationshipManagers(Moose::RelationshipManagerType input_rm_type)
{
  auto dg_kernel_params = DGKernelBase::validParams();
  addRelationshipManagers(input_rm_type, dg_kernel_params);
}

void
PrecursorAction::act()
{
  for (unsigned int op = 1; op <= _num_precursor_groups; ++op)
  {
    std::string var_name = _var_name_base + Moose::stringify(op);

    if (getParam<bool>("create_vars"))
    {
      // See whether we want to use an old solution
      if (getParam<bool>("init_from_file"))
      {
        if (_current_task == "check_copy_nodal_vars")
          _app.setExodusFileRestart(true);

        else if (_current_task == "copy_nodal_vars")
        {
          SystemBase * system = &_problem->getNonlinearSystemBase();
          system->addVariableToCopy(var_name, var_name, "LATEST");
        }
      }

      // Create variable names
      if (_current_task == "add_variable")
        addVariable(var_name);
    }

    // kernels
    if (_current_task == "add_kernel")
    {
      if (getParam<bool>("transient"))
        addTimeDerivative(var_name);

      addPrecursorSource(op, var_name);
      addPrecursorDecay(op, var_name);
    }

    // dg kernels
    else if (_current_task == "add_dg_kernel")
      addDGAdvection(var_name);

    // boundary conditions
    else if (_current_task == "add_bc")
    {
      addOutflowBC(var_name);

      if (getParam<bool>("loop_precursors"))
        addInflowBC(var_name);
    }

    // initial conditions
    else if (_current_task == "add_ic" && !getParam<bool>("init_from_file"))
      addInitialConditions(var_name);

    // postprocessors
    else if (_current_task == "add_postprocessor" && getParam<bool>("loop_precursors"))
    {
      // Set up postprocessors for calculating precursor conc at outlet
      // and receiving precursor conc at inlet from loop app
      addOutletPostprocessor(var_name);
      addInletPostprocessor(var_name);
    }

    // transfers
    else if (_current_task == "add_transfer" && getParam<bool>("loop_precursors") &&
        (!_is_loopapp))
    {
      // Set up MultiAppTransfer to simulate precursor looped flow into and
      // out of the reactor core
      addMultiAppTransfer(var_name);
    }
  }

  // Add outflow rate postprocessor for Navier-Stokes velocities in the main
  // app if precursors are looped
  if (_current_task == "add_postprocessor" && getParam<bool>("loop_precursors") &&
      isParamValid("uvel") && (!_is_loopapp))
    addCoolantOutflowPostprocessor();
}

void
PrecursorAction::addPrecursorSource(const unsigned & op, const std::string & var_name)
{
  if (getParam<bool>("eigen"))
  {
    InputParameters params = _factory.getValidParams("PrecursorEigenSource");
    setVarNameAndBlock(params, var_name);
    params.set<unsigned int>("num_groups") = _num_groups;
    params.set<unsigned int>("precursor_group_number") = op;
    std::vector<std::string> include = {"temperature", "group_fluxes"};
    params.applySpecificParameters(parameters(), include);
    params.set<bool>("use_exp_form") = getParam<bool>("nt_exp_form");

    std::string kernel_name = "PrecursorEigenSource_" + var_name + "_" + _object_suffix;
    _problem->addKernel("PrecursorEigenSource", kernel_name, params);
  }
  else
  {
    InputParameters params = _factory.getValidParams("PrecursorSource");
    setVarNameAndBlock(params, var_name);
    params.set<unsigned int>("num_groups") = _num_groups;
    params.set<unsigned int>("precursor_group_number") = op;
    std::vector<std::string> include = {"temperature", "group_fluxes"};
    params.applySpecificParameters(parameters(), include);
    params.set<bool>("use_exp_form") = getParam<bool>("nt_exp_form");
    params.set<Real>("eigenvalue_scaling") = getParam<Real>("eigenvalue_scaling");

    std::string kernel_name = "PrecursorSource_" + var_name + "_" + _object_suffix;
    _problem->addKernel("PrecursorSource", kernel_name, params);
  }
}

void
PrecursorAction::addPrecursorDecay(const unsigned & op, const std::string & var_name)
{
  InputParameters params = _factory.getValidParams("PrecursorDecay");
  setVarNameAndBlock(params, var_name);
  params.set<unsigned int>("precursor_group_number") = op;
  std::vector<std::string> include = {"temperature"};
  params.applySpecificParameters(parameters(), include);
  params.set<bool>("use_exp_form") = getParam<bool>("nt_exp_form");

  std::string kernel_name = "PrecursorDecay_" + var_name + "_" + _object_suffix;
  _problem->addKernel("PrecursorDecay", kernel_name, params);
}

void
PrecursorAction::addTimeDerivative(const std::string & var_name)
{
  InputParameters params = _factory.getValidParams("ScalarTransportTimeDerivative");
  setVarNameAndBlock(params, var_name);
  params.set<bool>("implicit") = true;
  params.set<bool>("use_exp_form") = getParam<bool>("nt_exp_form");

  std::string kernel_name = "ScalarTransportTimeDerivative_" + var_name + "_" + _object_suffix;
  _problem->addKernel("ScalarTransportTimeDerivative", kernel_name, params);
}

void
PrecursorAction::addDGAdvection(const std::string & var_name)
{
  if (getParam<bool>("constant_velocity_values"))
  {
    // if using constant and uniform velocity values
    InputParameters params = _factory.getValidParams("DGConvection");
    setVarNameAndBlock(params, var_name);
    RealVectorValue vel = {
        getParam<Real>("u_def"), getParam<Real>("v_def"), getParam<Real>("w_def")};
    params.set<RealVectorValue>("velocity") = vel;

    std::string kernel_name = "DGConvection_" + var_name + "_" + _object_suffix;
    _problem->addDGKernel("DGConvection", kernel_name, params);
  }
  else if (isParamValid("uvel")) // checks if Navier-Stokes velocities are provided
  {
    // if using Navier-Stokes velocities to couple to: (u, v, w)
    InputParameters params = _factory.getValidParams("DGCoupledAdvection");
    setVarNameAndBlock(params, var_name);
    params.set<std::vector<VariableName>>("uvel") = {getParam<NonlinearVariableName>("uvel")};
    if (isParamValid("vvel"))
      params.set<std::vector<VariableName>>("vvel") = {getParam<NonlinearVariableName>("vvel")};
    if (isParamValid("wvel"))
      params.set<std::vector<VariableName>>("wvel") = {getParam<NonlinearVariableName>("wvel")};
    std::string kernel_name = "DGCoupledAdvection_" + var_name + "_" + _object_suffix;
    _problem->addDGKernel("DGCoupledAdvection", kernel_name, params);
  }
  else
  {
    // if using prespecified functions
    InputParameters params = _factory.getValidParams("DGFunctionConvection");
    setVarNameAndBlock(params, var_name);
    params.set<FunctionName>("vel_x_func") = getParam<FunctionName>("u_func");
    params.set<FunctionName>("vel_y_func") = getParam<FunctionName>("v_func");
    params.set<FunctionName>("vel_z_func") = getParam<FunctionName>("w_func");
    std::string kernel_name = "DGFunctionConvection_" + var_name + "_" + _object_suffix;
    _problem->addDGKernel("DGFunctionConvection", kernel_name, params);
  }
}

void
PrecursorAction::addOutflowBC(const std::string & var_name)
{
  if (getParam<bool>("constant_velocity_values"))
  {
    // if using constant and uniform velocity values
    InputParameters params = _factory.getValidParams("OutflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("outlet_boundaries");
    RealVectorValue vel = {
        getParam<Real>("u_def"), getParam<Real>("v_def"), getParam<Real>("w_def")};
    params.set<RealVectorValue>("velocity") = vel;

    std::string bc_name = "OutflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("OutflowBC", bc_name, params);
  }
  else if (isParamValid("uvel")) // checks if Navier-Stokes velocities are provided
  {
    // if using navier stokes velocities to couple to: (u, v, w)
    InputParameters params = _factory.getValidParams("CoupledOutflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
      getParam<std::vector<BoundaryName>>("outlet_boundaries");
    params.set<std::vector<VariableName>>("uvel") = {getParam<NonlinearVariableName>("uvel")};
    if (isParamValid("vvel"))
      params.set<std::vector<VariableName>>("vvel") = {getParam<NonlinearVariableName>("vvel")};
    if (isParamValid("wvel"))
      params.set<std::vector<VariableName>>("wvel") = {getParam<NonlinearVariableName>("wvel")};

    std::string bc_name = "CoupledOutflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("CoupledOutflowBC", bc_name, params);
  }
  else
  {
    // if using prespecified functions
    InputParameters params = _factory.getValidParams("VelocityFunctionOutflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("outlet_boundaries");
    params.set<FunctionName>("vel_x_func") = getParam<FunctionName>("u_func");
    params.set<FunctionName>("vel_y_func") = getParam<FunctionName>("v_func");
    params.set<FunctionName>("vel_z_func") = getParam<FunctionName>("w_func");

    std::string bc_name = "VelocityFunctionOutflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("VelocityFunctionOutflowBC", bc_name, params);
  }
}

void
PrecursorAction::addInflowBC(const std::string & var_name)
{
  if (getParam<bool>("constant_velocity_values"))
  {
    // if using constant and uniform velocity values
    InputParameters params = _factory.getValidParams("PostprocessorInflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("inlet_boundaries");
    params.set<Real>("uu") = getParam<Real>("u_def");
    params.set<Real>("vv") = getParam<Real>("v_def");
    params.set<Real>("ww") = getParam<Real>("w_def");
    params.set<PostprocessorName>("postprocessor") =
        "Inlet_Average_" + var_name + "_" + _object_suffix;

    std::string bc_name = "PostprocessorInflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("PostprocessorInflowBC", bc_name, params);
  }
  else if (isParamValid("uvel")) // checks if Navier-Stokes velocities are provided
  {
    // if using navier stokes velocities to couple to: (u, v, w)
    InputParameters params = _factory.getValidParams("PostprocessorCoupledInflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("inlet_boundaries");
    params.set<std::vector<VariableName>>("uvel") = {getParam<NonlinearVariableName>("uvel")};
    if (isParamValid("vvel"))
      params.set<std::vector<VariableName>>("vvel") = {getParam<NonlinearVariableName>("vvel")};
    if (isParamValid("wvel"))
      params.set<std::vector<VariableName>>("wvel") = {getParam<NonlinearVariableName>("wvel")};
    params.set<PostprocessorName>("postprocessor") =
        "Inlet_Average_" + var_name + "_" + _object_suffix;

    std::string bc_name = "PostprocessorCoupledInflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("PostprocessorCoupledInflowBC", bc_name, params);
  }
  else
  {
    // if using prespecified functions
    InputParameters params = _factory.getValidParams("PostprocessorInflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("inlet_boundaries");
    params.set<PostprocessorName>("postprocessor") =
        "Inlet_Average_" + var_name + "_" + _object_suffix;

    std::string bc_name = "PostprocessorInflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("PostprocessorInflowBC", bc_name, params);
  }
}

void
PrecursorAction::addInitialConditions(const std::string & var_name)
{
  if (getParam<bool>("jac_test"))
  {
    InputParameters params = _factory.getValidParams("RandomIC");
    params.set<VariableName>("variable") = var_name;
    if (isParamValid("kernel_block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("kernel_block");
    else if (isParamValid("block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("block");
    params.set<Real>("min") = 0;
    params.set<Real>("max") = 1;

    std::string ic_name = "RandomIC_" + var_name;
    _problem->addInitialCondition("RandomIC", ic_name, params);
  }
}

void
PrecursorAction::addOutletPostprocessor(const std::string & var_name)
{
  // looping precursors requires connecting outlet of core problem
  // to the inlet of the loop subproblem. In addition, the outlet of the
  // loop must be connected to the core problem.
  if (getParam<bool>("constant_velocity_values"))
  {
    // Area-averaged precursor conc at outlet for constant and uniform flow
    std::string postproc_name = "Outlet_Average_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("SideAverageValue");
    std::vector<VariableName> varvec(1);
    varvec[0] = var_name;
    params.set<std::vector<VariableName>>("variable") = varvec;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("outlet_boundaries");
    params.set<std::vector<OutputName>>("outputs") = {"none"};
     _problem->addPostprocessor("SideAverageValue", postproc_name, params);
  }
  else if (isParamValid("uvel")) // checks if Navier-Stokes velocities are provided
  {
    {
      // Total flow-weighted precursor conc at outlet
      std::string postproc_name = "Outlet_Total_" + var_name + "_" + _object_suffix;
      InputParameters params = _factory.getValidParams("SideWeightedIntegralPostprocessor");
      std::vector<VariableName> varvec(1);
      varvec[0] = var_name;
      params.set<std::vector<VariableName>>("variable") = varvec;
      params.set<std::vector<BoundaryName>>("boundary") =
          getParam<std::vector<BoundaryName>>("outlet_boundaries");
      params.set<std::vector<OutputName>>("outputs") = {"none"};
      params.set<std::vector<VariableName>>("weight") =
          {getParam<NonlinearVariableName>("outlet_vel")};

      _problem->addPostprocessor("SideWeightedIntegralPostprocessor", postproc_name, params);
    }

    {
      // Area-averaged flow-weighted precursor conc at outlet
      std::string postproc_name = "Outlet_Average_" + var_name + "_" + _object_suffix;
      InputParameters params = _factory.getValidParams("DivisionPostprocessor");
      std::vector<VariableName> varvec(1);
      varvec[0] = var_name;
      params.set<PostprocessorName>("value1") = "Outlet_Total_" + var_name + "_" + _object_suffix;
      params.set<PostprocessorName>("value2") = "Coolant_Outflow_" + _object_suffix;
      params.set<std::vector<OutputName>>("outputs") = {"none"};
       _problem->addPostprocessor("DivisionPostprocessor", postproc_name, params);
    }
  }
}

void
PrecursorAction::addInletPostprocessor(const std::string & var_name)
{
  // Area-averaged precursor conc at inlet
  std::string postproc_name = "Inlet_Average_" + var_name + "_" + _object_suffix;
  InputParameters params = _factory.getValidParams("Receiver");
  params.set<ExecFlagEnum>("execute_on") = "nonlinear";
  params.set<std::vector<OutputName>>("outputs") = {"none"};

  _problem->addPostprocessor("Receiver", postproc_name, params);
}

void
PrecursorAction::addMultiAppTransfer(const std::string & var_name)
{
  // from main app to loop app
  {
    std::string transfer_name = "toloop_Transfer_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("MultiAppPostprocessorTransfer");
    params.set<MultiAppName>("to_multi_app") = getParam<MultiAppName>("multi_app");
    params.set<PostprocessorName>("from_postprocessor") =
        "Outlet_Average_" + var_name + "_" + _object_suffix;
    params.set<PostprocessorName>("to_postprocessor") =
        "Inlet_Average_" + var_name + "_" + _object_suffix;

    _problem->addTransfer("MultiAppPostprocessorTransfer", transfer_name, params);
  }

  // from loop app to main app
  {
    std::string transfer_name = "fromloop_Transfer_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("MultiAppPostprocessorTransfer");
    params.set<MultiAppName>("from_multi_app") = getParam<MultiAppName>("multi_app");
    params.set<PostprocessorName>("from_postprocessor") =
        "Outlet_Average_" + var_name + "_" + _object_suffix;
    params.set<PostprocessorName>("to_postprocessor") =
        "Inlet_Average_" + var_name + "_" + _object_suffix;
    params.set<MooseEnum>("reduction_type") = "average";

    _problem->addTransfer("MultiAppPostprocessorTransfer", transfer_name, params);
  }
}

void
PrecursorAction::addCoolantOutflowPostprocessor()
{
  std::string postproc_name = "Coolant_Outflow_" + _object_suffix;
  InputParameters params = _factory.getValidParams("SideWeightedIntegralPostprocessor");
  params.set<std::vector<VariableName>>("variable") =
      {getParam<NonlinearVariableName>("outlet_vel")};
  params.set<std::vector<BoundaryName>>("boundary") =
      getParam<std::vector<BoundaryName>>("outlet_boundaries");
  params.set<std::vector<OutputName>>("outputs") = {"none"};

  _problem->addPostprocessor("SideWeightedIntegralPostprocessor", postproc_name, params);
}

void
PrecursorAction::setVarNameAndBlock(InputParameters & params, const std::string & var_name)
{
  params.set<NonlinearVariableName>("variable") = var_name;
    if (isParamValid("kernel_block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("kernel_block");
    else if (isParamValid("block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("block");
}
