# flow_velocity=21.7 # cm/s. See MSRE-properties.ods
fuel_velocity_max_inlet = 43.4
fuel_sq_rad = 33.28
nt_scale = 1e13
ini_temp = 922
diri_temp = 922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  sss2_input = false
  account_delayed = true
  temperature = temp
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
[]

[Mesh]
  file = '3d_single_unit_msre_vol_fraction.msh'
  # file = jac_test.msh
[]

[Problem]
[]

[Variables]
  [group1]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1
    # scaling = 1e4
  []
  [group2]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1
    # scaling = 1e4
  []
  [temp]
    initial_condition = ${ini_temp}
    scaling = 1e-4
  []
[]

[Precursors]
  var_name_base = pre
  block = 'fuel'
  outlet_boundaries = 'fuel_tops'
  u_func = vel_x_func
  v_func = vel_y_func
  w_func = vel_z_func
  constant_velocity_values = false
  nt_exp_form = false
  family = MONOMIAL
  order = CONSTANT
  # jac_test = true
[]

[Kernels]
  # Neutronics
  [time_group1]
    type = NtTimeDerivative
    variable = group1
    group_number = 1
  []
  [diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  []
  [sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  []
  [fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
  []
  [delayed_group1]
    type = DelayedNeutronSource
    variable = group1
  []
  [inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  []
  [diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  []
  [sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  []
  [time_group2]
    type = NtTimeDerivative
    variable = group2
    group_number = 2
  []
  [fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
  []
  [inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  []

  # Temperature
  [temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  []
  [temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale = ${nt_scale}
    block = 'fuel'
  []
  # [./temp_source_mod]
  #   type = GammaHeatSource
  #   variable = temp
  #   gamma = .0144 # Cammi .0144
  #   block = 'moder'
  #   average_fission_heat = 'average_fission_heat'
  # [../]
  [temp_diffusion]
    type = MatDiffusion
    diffusivity = 'k'
    variable = temp
  []
  [temp_advection_fuel]
    type = VelocityFunctionTemperatureAdvection
    vel_x_func = vel_x_func
    vel_y_func = vel_y_func
    vel_z_func = vel_z_func
    variable = temp
    block = 'fuel'
  []
[]

[BCs]
  [vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides'
    variable = group1
  []
  [vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides'
    variable = group2
  []
  [temp_diri_cg]
    boundary = 'moder_bottoms fuel_bottoms moder_sides'
    type = FunctionDirichletBC
    function = 'temp_bc_func'
    variable = temp
  []
  [temp_advection_outlet]
    boundary = 'fuel_tops'
    type = VelocityFunctionTemperatureOutflowBC
    variable = temp
    vel_x_func = vel_x_func
    vel_y_func = vel_y_func
    vel_z_func = vel_z_func
  []
[]

[Functions]
  [temp_bc_func]
    type = ParsedFunction
    expression = '${ini_temp} - (${ini_temp} - ${diri_temp}) * tanh(t/1e-2)'
  []
  [vel_x_func]
    type = ParsedFunction
    expression = '0'
  []
  [vel_y_func]
    type = ParsedFunction
    expression = '0'
  []
  [vel_z_func]
    type = ParsedFunction
    expression = '${fuel_velocity_max_inlet} * cos(pi * x / (2 * ${fuel_sq_rad})) * cos(pi * y / (2 * ${fuel_sq_rad}))'
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_fuel_'
    interp_type = 'monotone_cubic'
    block = 'fuel'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  []
  [rho_fuel]
    type = DerivativeParsedMaterial
    property_name = rho
    expression = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    coupled_variables = 'temp'
    derivative_order = 1
    block = 'fuel'
  []
  [moder]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_mod_'
    interp_type = 'monotone_cubic'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'moder'
  []
  [rho_moder]
    type = DerivativeParsedMaterial
    property_name = rho
    expression = '1.86e-3 * exp(-1.8 * 1.0e-5 * (temp - 922))'
    coupled_variables = 'temp'
    derivative_order = 1
    block = 'moder'
  []
[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'asm      lu           1               preonly       1e-3'
  # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

  nl_max_its = 30
  l_max_its = 200

  #   dtmax = 1
  dtmin = 1e-5
  # dt = 1e-3
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-3
    cutback_factor = 0.4
    growth_factor = 1.2
    optimal_iterations = 20
    linear_iteration_ratio = 1000
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [group1_current]
    type = IntegralNewVariablePostprocessor
    variable = group1
    outputs = 'console csv'
  []
  [group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'console csv'
  []
  [multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'console csv'
  []
  [temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = 'fuel'
    outputs = 'csv console'
  []
  [temp_moder]
    type = ElementAverageValue
    variable = temp
    block = 'moder'
    outputs = 'csv console'
  []
  # [./average_fission_heat]
  #   type = AverageFissionHeat
  #   nt_scale = ${nt_scale}
  #   execute_on = 'linear nonlinear'
  #   outputs = 'console'
  #   block = 'fuel'
  # [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  csv = true
  exodus = true
[]

[Debug]
  show_var_residual_norms = true
[]

# [ICs]
#   [./temp_ic]
#     type = RandomIC
#     variable = temp
#     min = 922
#     max = 1022
#   [../]
#   [./group1_ic]
#     type = RandomIC
#     variable = group1
#     min = .5
#     max = 1.5
#   [../]
#   [./group2_ic]
#     type = RandomIC
#     variable = group2
#     min = .5
#     max = 1.5
#   [../]
# []
