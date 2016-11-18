flow_velocity=147 # Cammi 147 cm/s
inlet_temp=824
nt_scale=1e13

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  use_exp_form = false
  temperature = temp
  group_fluxes = 'group1 group2'
[../]

[Mesh]
  file = 'jac_test.msh'
[../]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  jac_test = true
[]

[Kernels]
  # Temperature
  [./temp_flow_fuel]
    block = 'fuel'
    type = MatINSTemperatureRZ
    variable = temp
    rho = 'rho'
    k = 'k'
    cp = 'cp'
    uz = ${flow_velocity}
  [../]
  [./temp_art_diff_fuel]
    block = 'fuel'
    type = ScalarAdvectionArtDiff
    v_def = ${flow_velocity}
    variable = temp
  [../]
  [./temp_flow_moder]
    block = 'moder'
    type = MatINSTemperatureRZ
    variable = temp
    rho = 'rho'
    k = 'k'
    cp = 'cp'
  [../]
  [./temp_source]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
  [../]
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
    rho = 'rho'
    cp = 'cp'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '../property_file_dir/msr2g_enrU_mod_953_fuel_interp_'
    prop_names = 'k rho cp'
    prop_values = '.0123 3.327e-3 1357' # Cammi 2011 at 908 K
  [../]
  [./moder]
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '../property_file_dir/msr2g_enrU_fuel_922_mod_interp_'
    prop_names = 'k rho cp'
    prop_values = '.312 1.843e-3 1760' # Cammi 2011 at 908 K
  [../]
[]

[BCs]
  # [./temp_inlet]
  #   boundary = 'fuel_bottom graphite_bottom'
  #   type = DirichletBC
  #   variable = temp
  #   value = ${inlet_temp}
  # [../]
  [./temp_outlet]
    boundary = 'fuel_top'
    type = MatINSTemperatureNoBCBC
    variable = temp
    k = 'k'
  [../]
  [./temp_art_diff_fuel]
    boundary = 'fuel_top'
    type = ScalarAdvectionArtDiffNoBCBC
    v_def = ${flow_velocity}
    variable = temp
  [../]
[]

[Problem]
  type = FEProblem
  coord_type = RZ
  kernel_coverage_check = false
[../]

[Executioner]
  type = Transient
  end_time = 10000

  nl_abs_tol = 4e-9
  trans_ss_check = true
  ss_check_tol = 4e-9

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor -snes_test_display'
  petsc_options_iname = '-snes_type'
  petsc_options_value = 'test'

  nl_max_its = 10
  l_max_its = 10

  dtmin = 1e-6
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
    growth_factor = 1.2
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
  [./out]
    type = Exodus
    execute_on = 'initial timestep_end'
  [../]
  [./dof_map]
    type = DOFMap
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[ICs]
  [./temp_ic]
    type = RandomIC
    variable = temp
    min = 850
    max = 950
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