ini_temp=922
diri_temp=922
nt_scale=1e13
temp = 922
[GlobalParams]
  num_groups = 4
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4'
  sss2_input = false
  account_delayed = false
  temperature = 922
  temp = 922
[]

flow_velocity = 21.7
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
#  [../]
#  [./group2]
#  [../]
#  [./group3]
#  [../]
#  [./group4]
#  [../]
#  #[./temp]
#  #  initial_condition = ${ini_temp}
#  #  scaling = 1e-4
#  #[../]
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
  [./vacuum_group3]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides'
    variable = group3
  [../]
  [./vacuum_group4]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides'
    variable = group4
  [../]

  #[./temp_diri_cg]
  #  boundary = 'moder_bottoms fuel_bottoms moder_sides'
  #  type = FunctionDirichletBC
  #  function = 'temp_bc_func'
  #  variable = temp
  #[../]
  #[./temp_advection_outlet]
  #  boundary = 'fuel_tops'
  #  type = TemperatureOutflowBC
  #  variable = temp
  #  velocity = '0 0 ${flow_velocity}'
  #[../]
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
    property_tables_root ='../../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gfuel_'
    interp_type = 'spline'
    block = 'fuel'
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gmoder_'
    interp_type = 'spline'
    block = 'moder'
  [../]
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'
  bx_norm = 'bnorm'
  k0 = 1.5
  pfactor = 1e-2
  l_max_its = 100
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = linear
  [../]
  [./tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  [../]
  [./group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  [../]
  [./group1max]
    type = NodalMaxValue
    variable = group1
    execute_on = timestep_end
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
  [./group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  [../]
  [./group2max]
    type = NodalMaxValue
    variable = group2
    execute_on = timestep_end
  [../]
  [./group2diff]
    type = ElementL2Diff
    variable = group2
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
[]


[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  csv = true
  exodus = true
[]

[Debug]
  show_var_residual_norms = true
[]
