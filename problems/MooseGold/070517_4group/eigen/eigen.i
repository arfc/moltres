[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  sss2_input = false
  account_delayed = true
[]

[Mesh]
  file = '../../033117_nts_temp_pre_parsed_mat/3d_msre_29x29_136.msh'
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

[PrecursorKernel]
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
    value = '${ini_temp} - (${ini_temp} - ${diri_temp}) * tanh(t/1e-2)'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root ='../../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gfuel'
    interp_type = 'spline'
    block = 'fuel'
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gmoder'
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
  petsc_options_value = 'asm      lu '
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]


[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  csv = true
  [./out]
    type = Exodus
    execute_on = 'final'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
