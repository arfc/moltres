flow_velocity=21.7 # cm/s. See MSRE-properties.ods
diri_temp=908
nt_scale=1e13
reactor_height=162.56
# global_temperature=temp
global_temperature=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  use_exp_form = true
  group_fluxes = 'group1 group2'
#   u_def = 0
#   v_def = 0
#   w_def = ${flow_velocity}
[../]

[Mesh]
  # file = 'msre_22x22_correct_vol_fraction.msh'
  # file = msre_cuboid_3x3_short_height.msh
  file = msre_cuboid_3x3.msh
[../]

[Problem]
  kernel_coverage_check = false
[]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'boundary'
  temp_scaling = 1e0
  nt_ic_function = 'nt_ic_func'
  create_temperature_var = false
  # temperature = ${global_temperature}
  temperature_value = ${global_temperature}
  dg_for_temperature = false
[]

[Materials]
  [./fuel]
    # type = CammiFuel
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '../property_file_dir/full_core_cuboid_msre_comp_fuel_data_func_of_fuel_temp_'
    prop_names = 'rho k cp'
    prop_values = '2.146e-3 .0553 1967' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
  [./moder]
    # type = CammiModerator
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '../property_file_dir/full_core_cuboid_msre_comp_mod_data_func_of_mod_temp_'
    prop_names = 'rho k cp'
    prop_values = '1.843e-3 .312 1760' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000
#   num_steps = 1

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'asm	  lu	       1	       preonly	     1e-3'

  nl_max_its = 30
  l_max_its = 100

  dtmin = 1e-5
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-3
    growth_factor = 1.025
    optimal_iterations = 20
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  csv = true
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]

[Debug]
  show_var_residual_norms = true
[]

[Functions]
  [./nt_ic_func]
    type = ParsedFunction
    value = '4/${reactor_height} * y * (1 - y/${reactor_height})'
  [../]
[]

[Postprocessors]
  [./group1_current]
    type = IntegralNewVariablePostprocessor
    variable = group1
    outputs = 'csv console'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'csv console'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'csv console'
  [../]
[]
