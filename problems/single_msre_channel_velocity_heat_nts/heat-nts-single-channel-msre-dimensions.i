# This input file tests outflow boundary conditions for the incompressible NS equations.
width = 3.048
height = 1.016
length = 162.56
nt_scale=1e13

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  sss2_input = false
  account_delayed = false
  temperature = temp
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  gamma = .0144 # Cammi .0144
  nt_scale = ${nt_scale}
[]

[Mesh]
  file = single_channel_msre_dimensions.msh
[]


[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
    # initial_condition = 1
    # scaling = 1e4
  [../]
  [./group2]
    order = FIRST
    family = LAGRANGE
    # initial_condition = 1
    # scaling = 1e4
  [../]
  [./temp]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./vel_x]
    block = 'fuel'
  [../]
  [./vel_y]
    block = 'fuel'
  [../]
  [./vel_z]
    block = 'fuel'
  [../]
  [./p]
    block = 'fuel'
  [../]
  [./power_density]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

# [Precursors]
#   var_name_base = pre
#   block = 'fuel'
#   outlet_boundaries = 'fuel_top'
#   u_func = vel_x_func
#   v_func = vel_y_func
#   w_func = vel_z_func
#   constant_velocity_values = false
#   nt_exp_form = false
#   family = MONOMIAL
#   order = CONSTANT
#   # jac_test = true
# []

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
  [../]
  # [./delayed_group1]
  #   type = DelayedNeutronSource
  #   variable = group1
  # [../]
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
  [./fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  [../]
  [./time_group2]
    type = NtTimeDerivative
    variable = group2
    group_number = 2
  [../]

  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./temp_fuel_transport]
    type = INSTemperature
    u = vel_x
    v = vel_y
    w = vel_z
    variable = temp
    block = 'fuel'
  [../]
  [./temp_mod_transport]
    type = MatDiffusion
    diffusivity = 'k'
    variable = temp
    block = 'moderator'
  [../]
  [./temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    block = 'fuel'
  [../]
  [./temp_source_mod]
    type = GammaHeatSource
    variable = temp
    block = 'moderator'
    average_fission_heat = 'average_fission_heat'
  [../]
[]

[AuxKernels]
  [./fuel]
    block = 'fuel'
    type = FissionHeatSourceTransientAux
    variable = power_density
  [../]
  [./moderator]
    block = 'moderator'
    type = ModeratorHeatSourceTransientAux
    average_fission_heat = 'average_fission_heat'
    variable = power_density
  [../]
[]

[BCs]
  [./temp_inlet]
    boundary = 'fuel_bottom'
    variable = temp
    value = 900
    type = DirichletBC
  [../]
  [./vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top moderator_bottoms moderator_tops'
    variable = group1
  [../]
  [./vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top moderator_bottoms moderator_tops'
    variable = group2
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
    prop_names = 'k cp rho'
    prop_values = '.0553 1967 2.146e-3' # Robertson MSRE technical report @ 922 K
    peak_power_density = peak_power_density
    controller_gain = 1e-4
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    prop_names = 'k cp rho'
    prop_values = '.312 1760 1.86e-3' # Cammi 2011 at 908 K
    block = 'moderator'
    peak_power_density = peak_power_density
    controller_gain = 0
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[Preconditioning]
  [./SMP_PJFNK]
    type = SMP
    full = true
    solve_type = PJFNK
    ksp_norm = none
  [../]
[]

# [Executioner]
#   # type = Steady
#   type = Transient
#   dt = 1
#   num_steps = 1
#   petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
#   petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3'
#   petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
#   # line_search = none
#   nl_rel_tol = 1e-8
#   nl_max_its = 50
#   l_max_its = 300
# []

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  # petsc_options_value = 'asm      lu           1               preonly       1e-3'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3'
  # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

  nl_max_its = 30
  l_max_its = 200

#   dtmax = 1
  dtmin = 1e-5
  # dt = 1e-3
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-3
    cutback_factor = 0.4
    growth_factor = 1.2
    optimal_iterations = 20
    linear_iteration_ratio = 1000
  [../]
[]

[Outputs]
  perf_graph = true
  exodus = true
  csv = true
  file_base = 'out'
[]

[Functions]
  [./nt_ic]
    type = ParsedFunction
    value = '10 * sin(pi * z / ${length})'
  [../]
  [./temp_ic]
    type = ParsedFunction
    value = '900 + 100 / ${length} * z'
  [../]
[]

[ICs]
  [./temp]
    type = FunctionIC
    variable = temp
    function = temp_ic
  [../]
  [./group1]
    type = FunctionIC
    variable = group1
    function = nt_ic
  [../]
  [./group2]
    type = FunctionIC
    variable = group2
    function = nt_ic
  [../]
[]

[MultiApps]
  [./sub]
    type = FullSolveMultiApp
    app_type = MoltresApp
    positions = '0 0 0'
    input_files = solution_aux_exodus.i
    execute_on = 'initial'
  [../]
[]

[Transfers]
  [./vel_x]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_x
    variable = vel_x
    execute_on = 'initial'
  [../]
  [./vel_y]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_y
    variable = vel_y
    execute_on = 'initial'
  [../]
  [./vel_z]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_z
    variable = vel_z
    execute_on = 'initial'
  [../]
  [./p]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = p
    variable = p
    execute_on = 'initial'
  [../]
[]

[Postprocessors]
  [./group1_current]
    type = IntegralNewVariablePostprocessor
    variable = group1
    outputs = 'console csv'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'console csv'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'console csv'
  [../]
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = 'fuel'
    outputs = 'csv console'
  [../]
  [./temp_moder]
    type = ElementAverageValue
    variable = temp
    block = 'moderator'
    outputs = 'csv console'
  [../]
  [./average_fission_heat]
    type = AverageFissionHeat
    execute_on = 'linear nonlinear'
    outputs = 'console'
    block = 'fuel'
  [../]
  [./peak_power_density]
    type = ElementExtremeValue
    value_type = max
    variable = power_density
    execute_on = 'linear nonlinear timestep_begin'
  [../]
[]
