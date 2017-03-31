reactor_height=162.56
# global_temperature=temp
global_temperature=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  use_exp_form = false
  group_fluxes = 'group1 group2'
[../]

[Mesh]
  file = 'msre_22x22_correct_vol_fraction.msh'
[../]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'fuel_bottoms fuel_tops moder_bottoms moder_tops fuel_sides'
  temp_scaling = 1e0
  nt_ic_function = 'nt_ic_func'
  create_temperature_var = false
  # temperature = ${global_temperature}
  temperature_value = ${global_temperature}
  dg_for_temperature = false
  eigen = true
[]

[Materials]
  [./fuel]
    # type = CammiFuel
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '../property_file_dir/B1_full_core_cuboid_msre_comp_fuel_data_func_of_fuel_temp_'
    prop_names = 'rho k cp'
    prop_values = '2.146e-3 .0553 1967' # Robertson MSRE technical report @ 922 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
  [./moder]
    # type = CammiModerator
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '../property_file_dir/B1_full_core_cuboid_msre_comp_mod_data_func_of_mod_temp_'
    prop_names = 'rho k cp'
    prop_values = '1.843e-3 .312 1760' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
[]

[Executioner]
  # type = NonlinearEigen
  # free_power_iterations = 4
  # source_abs_tol = 1e-12
  # source_rel_tol = 1e-8
  # output_after_power_iterations = true

  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'

  bx_norm = 'bnorm'
  k0 = 1.0
  pfactor = 1e-2
  l_max_its = 100

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type'
  petsc_options_value = 'asm lu'
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
  [./bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    group_fluxes = 'group1 group2'
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
  [./group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  [../]
  [./group1max]
    type = NodalMaxValue
    variable = group1
    execute_on = timestep_end
  [../]
  [./group2max]
    type = NodalMaxValue
    variable = group2
    execute_on = timestep_end
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
[]
