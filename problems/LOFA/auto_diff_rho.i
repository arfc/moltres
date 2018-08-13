# take in steady state variable from the other core, and run a pump coastdown transient
# here, flow_velocity is only the initial flow velocity
flow_velocity=21.7 # cm/s. See MSRE-properties.ods
tau = 5 # pump coastdown time constant
nt_scale=1e13
ini_temp=922
diri_temp=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  temperature = temp
  sss2_input = false
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  account_delayed = true
[]

[Mesh]
  file = 'steady/auto_diff_rho.e'
[../]

[Problem]
  coord_type = RZ
[]

[Precursors]
  [./pres]
    var_name_base = pre
    block = 'fuel'
    outlet_boundaries = 'fuel_tops'
    constant_velocity_values = false
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    u_func = nullFunc
    v_func = coastDownFunc
    w_func = nullFunc
    init_from_file = true
  [../]
[]

[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
    scaling = 1e4
    initial_from_file_var = group1
    initial_from_file_timestep = LATEST [../]
  [./group2]
    order = FIRST
    family = LAGRANGE
    scaling = 1e4
    initial_from_file_var = group2
    initial_from_file_timestep = LATEST
  [../]
  [./temp]
    scaling = 1e-4
    order = FIRST
    family = LAGRANGE
    initial_from_file_var = temp
    initial_from_file_timestep = LATEST
  [../]
[]


[Kernels]
  # Neutronics
  [./time_group1]
    type = NtTimeDerivative
    variable = group1
    group_number = 1
  [../]
  [./diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  [../]
  [./sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  [../]
  [./fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    block = 'fuel'
  [../]
  [./delayed_group1]
    type = DelayedNeutronSource
    variable = group1
    block = 'fuel'
  [../]
  [./inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  [../]
  [./diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  [../]
  [./sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  [../]
  [./time_group2]
    type = NtTimeDerivative
    variable = group2
    group_number = 2
  [../]
  [./fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
    block = 'fuel'
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  [../]

  # Temperature
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
    block = 'fuel'
  [../]
  [./temp_source_mod]
     type = GammaHeatSource
     variable = temp
     gamma = .0144 # Cammi .0144
     block = 'moder'
     average_fission_heat = 'average_fission_heat'
  [../]
  [./temp_diffusion]
    type = MatDiffusion
    D_name = 'k'
    variable = temp
  [../]
  [./temp_advection_fuel]
    type = CtrlConservativeTemperatureAdvection
    u_val = 0
    v_val = ${flow_velocity} # this will be changed in ctrls block
    w_val = 0
    variable = temp
    block = 'fuel'
  [../]
[]

[BCs]
  [./vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
    variable = group1
  [../]
  [./vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
    variable = group2
  [../]
  [./temp_postpr_inlet]
    boundary = 'moder_bottoms fuel_bottoms outer_wall'
    type = PostprocessorDirichletBC
    variable = temp
    postprocessor = inlet_mean_temp
  [../]
  #[./temp_postpr_inlet]
  #  boundary = 'moder_bottoms fuel_bottoms outer_wall'
  #  type = DirichletBC
  #  variable = temp
  #  value = 922
  #[../]
  [./temp_advection_outlet]
    boundary = 'fuel_tops'
    type = VelocityFunctionTemperatureOutflowBC
    variable = temp
    vel_x_func = nullFunc
    vel_y_func = coastDownFunc
    vel_z_func = nullFunc
  [../]
[]

[Functions]
  [./temp_bc_func]
    type = ParsedFunction
    value = '${ini_temp} - (${ini_temp} - ${diri_temp}) * tanh(t/1e-2)'
  [../]
  [./nullFunc]
    type = ParsedFunction
    value = '0'
  [../]
  [./coastDownFunc]
    type = ParsedFunction
    value = '${flow_velocity} * exp(-t / ${tau})'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '/home/gav/projects/moltres/property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
  [../]
  [./rho_fuel]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'fuel'
  [../]
  # fuel cp function
  [./cp_fuel]
    type = DerivativeParsedMaterial
    f_name = cp
    function = '1967 + 1e-16 * temp'
    args = 'temp'
    derivative_order = 1
    block = 'fuel'
  [../]
  # fuel k function
  [./k_fuel]
    type = DerivativeParsedMaterial
    f_name = k
    function = '0.0553 + 1e-16 * temp'
    args = 'temp'
    derivative_order = 1
    block = 'fuel'
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '/home/gav/projects/moltres/property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    block = 'moder'
  [../]
  [./rho_moder]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '1.86e-3 * exp(-1.8 * 1.0e-5 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'moder'
  [../]
  # moder cp fcn
  [./cp_moder]
    type = DerivativeParsedMaterial
    f_name = cp
    function = '1760 + 1e-16 * temp'
    args = 'temp'
    derivative_order = 1
    block = 'moder'
  [../]
  # moder k fcn
  [./k_moder]
    type = DerivativeParsedMaterial
    f_name = k
    function = '.312 + 1e-16 * temp'
    args = 'temp'
    derivative_order = 1
    block = 'moder'
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000
  nl_abs_tol = 3e-6

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type' #  -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu' #       NONZERO               1e-10                   preonly   1e-3'
  line_search = 'none'
   # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

  nl_max_its = 30
  l_max_its = 100

  dtmin = 1e-5
  # dt = 1e-3
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-3
    cutback_factor = 0.4
    growth_factor = 1.04
    optimal_iterations = 5
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    # ksp_norm = none
  [../]
[]

[Postprocessors]
  [./group1_current]
    type = IntegralNewVariablePostprocessor
    variable = group1
    outputs = 'console exodus'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'console exodus'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'console exodus'
  [../]
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = 'fuel'
    outputs = 'exodus console'
  [../]
  [./temp_moder]
    type = ElementAverageValue
    variable = temp
    block = 'moder'
    outputs = 'exodus console'
  [../]
  [./coreEndTemp]
    type = SideAverageValue
    variable = temp
    boundary = 'fuel_tops'
    outputs = 'exodus console'
    execute_on = 'timestep_begin'
  [../]
  #multiapp
  [./inlet_mean_temp]
    type = Receiver
    execute_on = 'initial timestep_begin'
  [../]
  [./average_fission_heat]
    type = AverageFissionHeat
    nt_scale = ${nt_scale}
    execute_on = 'linear nonlinear'
    outputs = 'console'
    block = 'fuel'
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./exodus]
    type = Exodus
    file_base = 'auto_diff_rho'
    execute_on = 'timestep_end'
  [../]
[]

[Controls]
  [./flowControl]
    type = RealFunctionControl
    parameter = '*/*/v_val'
    function = coastDownFunc
    execute_on = 'initial timestep_begin'
  [../]
[]

[MultiApps]
  [./loopApp]
    type = TransientMultiApp
    app_type = MoltresApp
    execute_on = 'initial timestep_begin'
    positions = '100.0 100.0 0.0'
    input_files = 'sub.i'
 [../]
[]

# connect inlet and outlet to multiapp
[Transfers]
  [./from_loop]
    type = MultiAppPostprocessorTransfer
    multi_app = loopApp
    from_postprocessor = loopEndTemp
    to_postprocessor = inlet_mean_temp
    direction = from_multiapp
    reduction_type = maximum
  [../]
  [./to_loop]
    type = MultiAppPostprocessorTransfer
    multi_app = loopApp
    from_postprocessor = coreEndTemp
    to_postprocessor = coreEndTemp
    direction = to_multiapp
 [../]
[]
