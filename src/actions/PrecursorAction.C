#include "PrecursorAction.h"
#include "Factory.h"
#include "Parser.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "NonlinearSystemBase.h"

template <>
InputParameters
validParams<PrecursorAction>()
{
  InputParameters params = validParams<AddVariableAction>();
  params.addRequiredParam<unsigned int>("num_precursor_groups",
                                        "specifies the total number of precursors to create");
  params.addRequiredParam<std::string>("var_name_base", "specifies the base name of the variables");
  params.addRequiredCoupledVar("temperature", "Name of temperature variable");
  params.addParam<VariableName>("u", "Name of x-component of velocity");
  params.addParam<VariableName>("v", "Name of y-component of velocity");
  params.addParam<VariableName>("w", "Name of z-component of velocity");
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
  return params;
}

PrecursorAction::PrecursorAction(const InputParameters & params)
  : AddVariableAction(params),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _var_name_base(getParam<std::string>("var_name_base")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _object_suffix(getParam<std::string>("object_suffix")),
    _multi_app(getParam<MultiAppName>("multi_app"))
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
      //
      // See whether we want to use an old solution
      //
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

      //
      // Create variable names
      //

      if (_current_task == "add_variable")
        addVariable(var_name);
    }

    if (_current_task == "add_kernel")
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

      //
      // Set up PrecursorDecay kernels
      //

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

      //
      // Set up TimeDerivative kernels
      //
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

        std::string kernel_name =
            "ScalarTransportTimeDerivative_" + var_name + "_" + _object_suffix;
        _problem->addKernel("ScalarTransportTimeDerivative", kernel_name, params);
      }
    }

    if (_current_task == "add_dg_kernel")
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

    if (_current_task == "add_bc")
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

        std::string kernel_name = "OutflowBC_" + var_name + "_" + _object_suffix;
        _problem->addBoundaryCondition("OutflowBC", kernel_name, params);
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

        std::string kernel_name = "VelocityFunctionOutflowBC_" + var_name + "_" + _object_suffix;
        _problem->addBoundaryCondition("VelocityFunctionOutflowBC", kernel_name, params);
      }
      // INFLOW
      if (getParam<bool>("loop_precs"))
      {
          // this SHOULD work for both constant and nonconstant flows as long as
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
              "Inlet_SideAverageValue"+
              var_name + "_" + _object_suffix;

          // OK, Alex called it kernel_name despite it being a BC,
          // and it doesn't matter, but let's follow suit.
          std::string kernel_name = "PostprocessorInflowBC_" + var_name + 
              "_" + _object_suffix;
          _problem->addBoundaryCondition("PostprocessorInflowBC", kernel_name, params);
      }

      // fixed concentration input: no use at the moment so commented out
      // {
      //   // requires new BC: PostprocessorInflowBC
      //   InputParameters params = _factory.getValidParams("InflowBC");
      //   params.set<NonlinearVariableName>("variable") = var_name;
      //   params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName>
      //   >("inlet_boundaries");
      //   RealVectorValue vel = {getParam<Real>("u_def"), getParam<Real>("v_def"),
      //   getParam<Real>("w_def")};
      //   params.set<RealVectorValue>("velocity") = vel;
      //   params.set<Real>("inlet_conc") = 1.;

      //   std::string kernel_name = "InflowBC_" + var_name;
      //   _problem->addBoundaryCondition("InflowBC", kernel_name, params);
      // }
    }

    // Set up ICs

    if (_current_task == "add_ic" && !getParam<bool>("init_from_file"))
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

    if (_current_task == "add_postprocessor" && getParam<bool>("loop_precs"))
    {
        // looping precursors requires connecting outlet of core problem
        // to the inlet of the loop subproblem. In addition, the outlet of the
        // loop must be connected to the core problem.
        std::string postproc_name = "Outlet_SideAverageValue_"+var_name+"_"+_object_suffix;
        InputParameters params = _factory.getValidParams("SideAverageValue");
        params.set<NonlinearVariableName>("variable") = var_name;
        params.set<std::vector<BoundaryName>>("boundary") =
            getParam<std::vector<BoundarName>>("outlet_boundaries");
        
        _problem->addPostprocessor("SideAverageValue", postproc_name, params);
        
    }

    if (_current_task == "add_transfer" && getParam<bool>("loop_precs") &&
            !getParam<bool>("is_loopapp"))
    {
        // from main app to loop app
        {
            std::string transfer_name = "toloop_Transfer_" + var_name + "_"+_object_suffix;
            InputParameters params = _factory.getValidParams("MultiAppPostprocessorTransfer");
            params.set<MultiAppName>("multi_app") = _multi_app;
            params.set<PostprocessorName>("from_postprocessor") = "Outlet_SideAverageValue_"+var_name+"_"+_object_suffix;
            params.set<PostprocessorName>("to_postprocessor") = "Inlet_SideAverageValue_"+var_name+"_"+_object_suffix;
            params.set<MooseEnum>("direction") = "to_multiapp";
        }

        // from loop app to main app
        {
            std::string transfer_name = "fromloop_Transfer_" + var_name + "_" + _object_suffix;
            InputParameters params = _factory.getValidParams("MultiAppPostprocessorTransfer");
            params.set<MultiAppName>("multi_app") = _multi_app;
            params.set<PostprocessorName>("from_postprocessor") = "Outlet_SideAverageValue_"+var_name+"_"+_object_suffix;
            params.set<PostprocessorName>("to_postprocessor") = "Inlet_SideAverageValue_"+var_name+"_"+_object_suffix;
            params.set<MooseEnum>("direction") = "from_multiapp";
        }
    }
  }
}
