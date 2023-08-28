[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  sss2_input = false
  account_delayed = false
[]

[Mesh]
  file = '3d_msre_29x29_136.msh'
  # file = jac_test.msh
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'moder_sides fuel_bottoms fuel_tops moder_bottoms moder_tops'
  create_temperature_var = false
  eigen = true
[]

#[Variables]
#  [./group1]
#    order = FIRST
#    family = LAGRANGE
#    initial_condition = 1
#    scaling = 1e4
#  [../]
#  [./group2]
#    order = FIRST
#    family = LAGRANGE
#    initial_condition = 1
#    scaling = 1e4
#  [../]
#  [./temp]
#    initial_condition = ${ini_temp}
#    scaling = 1e-4
#  [../]
#[]

[Precursors]
  var_name_base = pre
  block = 'fuel'
  outlet_boundaries = 'fuel_tops'
  u_def = 0
  v_def = 0
  w_def = ${flow_velocity}
  nt_exp_form = false
  family = MONOMIAL
  order = CONSTANT
  jac_test = true
[]

[BCs]
  [./vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides'
    variable = group1
  [../]
  [./vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides'
    variable = group2
  [../]
  [./temp_diri_cg]
    boundary = 'moder_bottoms fuel_bottoms moder_sides'
    type = FunctionDirichletBC
    function = 'temp_bc_func'
    variable = temp
  [../]
  [./temp_advection_outlet]
    boundary = 'fuel_tops'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 0 ${flow_velocity}'
  [../]
[]

[Functions]
  [./temp_bc_func]
    type = ParsedFunction
    expression = '${ini_temp} - (${ini_temp} - ${diri_temp}) * tanh(t/1e-2)'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    block = 'moder'
  [../]
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  #petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_iname = '-pc_type -sub_pc_type'
  petsc_options_value = 'asm      lu ' #1               preonly       1e-3'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

#[Postprocessors]
#  [./group1_current]
#    type = IntegralNewVariablePostprocessor
#    variable = group1
#    outputs = 'console csv'
#  [../]
#  [./group1_old]
#    type = IntegralOldVariablePostprocessor
#    variable = group1
#    outputs = 'console csv'
#  [../]
#  [./multiplication]
#    type = DivisionPostprocessor
#    value1 = group1_current
#    value2 = group1_old
#    outputs = 'console csv'
#  [../]
#  [./temp_fuel]
#    type = ElementAverageValue
#    variable = temp
#    block = 'fuel'
#    outputs = 'csv console'
#  [../]
#  [./temp_moder]
#    type = ElementAverageValue
#    variable = temp
#    block = 'moder'
#    outputs = 'csv console'
#  [../]
#  # [./average_fission_heat]
#  #   type = AverageFissionHeat
#  #   nt_scale = ${nt_scale}
#  #   execute_on = 'linear nonlinear'
#  #   outputs = 'console'
#  #   block = 'fuel'
#  # [../]
#[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  # csv = true
  [./out]
    type = Exodus
    execute_on = 'final'
  [../]
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
