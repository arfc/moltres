#include "PrecursorAction.h"
#include "Factory.h"
#include "Parser.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "NonlinearSystemBase.h"

registerMooseAction("MoltresApp", PrecursorAction, "add_kernel");

registerMooseAction("MoltresApp", PrecursorAction, "add_postprocessor");

registerMooseAction("MoltresApp", PrecursorAction, "add_bc");

registerMooseAction("MoltresApp", PrecursorAction, "add_variable");

registerMooseAction("MoltresApp", PrecursorAction, "add_ic");

registerMooseAction("MoltresApp", PrecursorAction, "add_dg_kernel");

registerMooseAction("MoltresApp", PrecursorAction, "add_transfer");

registerMooseAction("MoltresApp", PrecursorAction, "check_copy_nodal_vars");

registerMooseAction("MoltresApp", PrecursorAction, "copy_nodal_vars");

template <>
InputParameters
validParams<PrecursorAction>()
{
  InputParameters params = validParams<AddVariableAction>();
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
                        true,
                        "Whether concentrations should be in an expotential/logarithmic format.");
  params.addParam<bool>("jac_test",
                        false,
                        "Whether we're testing the Jacobian and should use some "
                        "random initial conditions for the precursors.");
  params.addParam<Real>("prec_scale", "The amount by which the neutron fluxes are scaled.");
  params.addParam<bool>("transient", true, "Whether to run a transient simulation.");
  params.addParam<bool>(
      "init_from_file", false, "Whether to initialize the precursors from a file.");
  params.addParam<bool>("create_vars", true, "Whether this action should create the variables.");
  params.addParam<bool>("loop_precs", false, "Whether precursors are circulated in salt loop.");
  params.addParam<std::string>(
      "object_suffix",
      "",
      "An optional suffix string that can be helpful to avoid object name crashing.");
  params.addParam<std::vector<SubdomainName>>("kernel_block",
                                              "Kernel bock can be different from block.");
  params.addParam<MultiAppName>("multi_app", "Multiapp name for looping precursors.");
  params.addParam<bool>("is_loopapp", "if circulating precursors, whether this is loop app");
  params.addParam<bool>("eigen", false, "whether neutronics is in eigenvalue calculation mode");
  return params;
}

PrecursorAction::PrecursorAction(const InputParameters & params)
  : AddVariableAction(params),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _var_name_base(getParam<std::string>("var_name_base")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _object_suffix(getParam<std::string>("object_suffix"))
{
  if (getParam<bool>("loop_precs"))
  {
    if (!params.isParamSetByUser("inlet_boundaries"))
      mooseError("Looping precursors requires specification of inlet_boundaries.");
    if (!params.isParamValid("multi_app"))
      mooseError("Looping precursors requires a multiapp that governs the loop.");
  }
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
          _app.setFileRestart() = true;

        if (_current_task == "copy_nodal_vars")
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
      kernelAct(op, var_name);

    // dg kernels
    if (_current_task == "add_dg_kernel")
      dgKernelAct(var_name);

    // bcs
    if (_current_task == "add_bc")
      bcAct(var_name);

    // ics
    if (_current_task == "add_ic" && !getParam<bool>("init_from_file"))
      icAct(var_name);

    // postprocessors
    if (_current_task == "add_postprocessor" && getParam<bool>("loop_precs"))
      postAct(var_name);

    // transfers
    if (_current_task == "add_transfer" && getParam<bool>("loop_precs") &&
        !getParam<bool>("is_loopapp"))
      transferAct(var_name);
  }
}

void
PrecursorAction::kernelAct(const unsigned & op, const std::string & var_name)
{
  // Set up PrecursorSource kernels
  {
    InputParameters params = _factory.getValidParams("PrecursorSource");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<unsigned int>("num_groups") = _num_groups;
    params.set<unsigned int>("precursor_group_number") = op;
    std::vector<std::string> include = {"temperature", "group_fluxes"};
    params.applySpecificParameters(parameters(), include);
    if (isParamValid("kernel_block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("kernel_block");
    else if (isParamValid("block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("block");
    params.set<bool>("use_exp_form") = getParam<bool>("nt_exp_form");

    std::string kernel_name = "PrecursorSource_" + var_name + "_" + _object_suffix;
    _problem->addKernel("PrecursorSource", kernel_name, params);
  }

  // Set up PrecursorDecay kernels
  {
    InputParameters params = _factory.getValidParams("PrecursorDecay");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<unsigned int>("precursor_group_number") = op;
    std::vector<std::string> include = {"temperature"};
    params.applySpecificParameters(parameters(), include);
    if (isParamValid("kernel_block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("kernel_block");
    else if (isParamValid("block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("block");
    params.set<bool>("use_exp_form") = false;

    std::string kernel_name = "PrecursorDecay_" + var_name + "_" + _object_suffix;
    _problem->addKernel("PrecursorDecay", kernel_name, params);
  }

  // Set up TimeDerivative kernels
  if (getParam<bool>("transient"))
  {
    InputParameters params = _factory.getValidParams("ScalarTransportTimeDerivative");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<bool>("implicit") = true;
    if (isParamValid("kernel_block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("kernel_block");
    else if (isParamValid("block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("block");
    params.set<bool>("use_exp_form") = false;

    std::string kernel_name = "ScalarTransportTimeDerivative_" + var_name + "_" + _object_suffix;
    _problem->addKernel("ScalarTransportTimeDerivative", kernel_name, params);
  }
}

void
PrecursorAction::dgKernelAct(const std::string & var_name)
{
  if (getParam<bool>("constant_velocity_values"))
  {
    InputParameters params = _factory.getValidParams("DGConvection");
    params.set<NonlinearVariableName>("variable") = var_name;
    if (isParamValid("kernel_block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("kernel_block");
    else if (isParamValid("block"))
      params.set<std::vector<SubdomainName>>("block") =
          getParam<std::vector<SubdomainName>>("block");
    RealVectorValue vel = {
        getParam<Real>("u_def"), getParam<Real>("v_def"), getParam<Real>("w_def")};
    params.set<RealVectorValue>("velocity") = vel;

    std::string kernel_name = "DGConvection_" + var_name + "_" + _object_suffix;
    _problem->addDGKernel("DGConvection", kernel_name, params);
  }
  else
  {
    if (isParamValid("uvel"))
    {
      // this stuff happens if you have navier stokes velocities to couple to, u,v,w.
      InputParameters params = _factory.getValidParams("DGCoupledAdvection");
      params.set<NonlinearVariableName>("variable") = var_name;
      if (isParamValid("kernel_block"))
        params.set<std::vector<SubdomainName>>("block") =
            getParam<std::vector<SubdomainName>>("kernel_block");
      else if (isParamValid("block"))
        params.set<std::vector<SubdomainName>>("block") =
            getParam<std::vector<SubdomainName>>("block");
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
      // this stuff happens if no navier stokes velocities are available:
      // use prespecified functions.
      InputParameters params = _factory.getValidParams("DGFunctionConvection");
      params.set<NonlinearVariableName>("variable") = var_name;
      if (isParamValid("kernel_block"))
        params.set<std::vector<SubdomainName>>("block") =
            getParam<std::vector<SubdomainName>>("kernel_block");
      else if (isParamValid("block"))
        params.set<std::vector<SubdomainName>>("block") =
            getParam<std::vector<SubdomainName>>("block");
      params.set<FunctionName>("vel_x_func") = getParam<FunctionName>("u_func");
      params.set<FunctionName>("vel_y_func") = getParam<FunctionName>("v_func");
      params.set<FunctionName>("vel_z_func") = getParam<FunctionName>("w_func");
      std::string kernel_name = "DGFunctionConvection_" + var_name + "_" + _object_suffix;
      _problem->addDGKernel("DGFunctionConvection", kernel_name, params);
    }
  }
}

void
PrecursorAction::bcAct(const std::string & var_name)
{
  // OUTFLOW
  if (getParam<bool>("constant_velocity_values"))
  {
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
  else if (isParamValid("uvel"))
  {
    mooseWarning("There's currently no DG transport OutflowBC using N-S velocities."
                 "Assuming reactor geometry like MSFR w/ no outflow.");
  }
  else
  {
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
  // INFLOW
  if (getParam<bool>("loop_precs"))
  {
    // this will work for both constant and nonconstant flows as long as
    // nonconstant flows implemented in the Controls module by
    // setting values called uu, vv, ww.
    if (!getParam<bool>("constant_velocity_values"))
      mooseError("Variable, looped precursor advection requires that variable"
                 "velocity has the values uu, vv, ww set through the controls"
                 "module, NOT simply specifying functions through the"
                 "precursors block.");
    InputParameters params = _factory.getValidParams("PostprocessorInflowBC");
    params.set<NonlinearVariableName>("variable") = var_name;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("inlet_boundaries");
    params.set<Real>("uu") = getParam<Real>("u_def");
    params.set<Real>("vv") = getParam<Real>("v_def");
    params.set<Real>("ww") = getParam<Real>("w_def");
    params.set<PostprocessorName>("postprocessor") =
        "Inlet_SideAverageValue_" + var_name + "_" + _object_suffix;

    std::string bc_name = "PostprocessorInflowBC_" + var_name + "_" + _object_suffix;
    _problem->addBoundaryCondition("PostprocessorInflowBC", bc_name, params);
  }
}

void
PrecursorAction::icAct(const std::string & var_name)
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
PrecursorAction::postAct(const std::string & var_name)
{
  // looping precursors requires connecting outlet of core problem
  // to the inlet of the loop subproblem. In addition, the outlet of the
  // loop must be connected to the core problem.
  {
    std::string postproc_name = "Outlet_SideAverageValue_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("SideAverageValue");
    std::vector<VariableName> varvec(1);
    varvec[0] = var_name;
    params.set<std::vector<VariableName>>("variable") = varvec;
    params.set<std::vector<BoundaryName>>("boundary") =
        getParam<std::vector<BoundaryName>>("outlet_boundaries");
    params.set<std::vector<OutputName>>("outputs") = {"none"};

    _problem->addPostprocessor("SideAverageValue", postproc_name, params);
  }
  {
    std::string postproc_name = "Inlet_SideAverageValue_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("Receiver");
    params.set<ExecFlagEnum>("execute_on") = "nonlinear";
    params.set<std::vector<OutputName>>("outputs") = {"none"};

    _problem->addPostprocessor("Receiver", postproc_name, params);
  }
}

void
PrecursorAction::transferAct(const std::string & var_name)
{
  // from main app to loop app
  {
    std::string transfer_name = "toloop_Transfer_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("MultiAppPostprocessorTransfer");
    params.set<MultiAppName>("multi_app") = getParam<MultiAppName>("multi_app");
    params.set<PostprocessorName>("from_postprocessor") =
        "Outlet_SideAverageValue_" + var_name + "_" + _object_suffix;
    params.set<PostprocessorName>("to_postprocessor") =
        "Inlet_SideAverageValue_" + var_name + "_" + _object_suffix;
    params.set<MooseEnum>("direction") = "to_multiapp";

    _problem->addTransfer("MultiAppPostprocessorTransfer", transfer_name, params);
  }

  // from loop app to main app
  {
    std::string transfer_name = "fromloop_Transfer_" + var_name + "_" + _object_suffix;
    InputParameters params = _factory.getValidParams("MultiAppPostprocessorTransfer");
    params.set<MultiAppName>("multi_app") = getParam<MultiAppName>("multi_app");
    params.set<PostprocessorName>("from_postprocessor") =
        "Outlet_SideAverageValue_" + var_name + "_" + _object_suffix;
    params.set<PostprocessorName>("to_postprocessor") =
        "Inlet_SideAverageValue_" + var_name + "_" + _object_suffix;
    params.set<MooseEnum>("direction") = "from_multiapp";
    params.set<MooseEnum>("reduction_type") = "average";

    _problem->addTransfer("MultiAppPostprocessorTransfer", transfer_name, params);
  }
}
