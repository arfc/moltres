[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[../]


[Kernels]
  # Temperature
  [./temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale = 1
    group_fluxes = '1 1'
    block = 'fuel'
  [../]
  [./temp_source_mod]
    type = GammaHeatSource
    variable = temp
    gamma = 1 # Cammi .0144
    block = 'moder'
    average_fission_heat = 'average_fission_heat'
  [../]
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
[]

[Materials]
  [./fuel]
    type = CammiFuel
    block = '0'
    property_tables_root = '../property_file_dir/newt_fuel_'
    prop_names = 'cp'
    prop_values = '1357' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = temp
  [../]
  [./moder]
    type = CammiModerator
    block = '1'
    property_tables_root = '../property_file_dir/newt_mod_'
    prop_names = 'rho cp'
    prop_values = '1.843e-3 1760' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = temp
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda -pc_factor_mat_solver_package'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3 mumps'

  nl_max_its = 20
  l_max_its = 10

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
  [./out]
    type = Exodus
    execute_on = 'initial timestep_end'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[ICs]
  [./temp_all_ic_func]
    type = ConstantIC
    variable = temp
    value = 824
  [../]
[]

[Postprocessors]
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = '0'
    outputs = 'csv console'
  [../]
  [./temp_moder]
    type = ElementAverageValue
    variable = temp
    block = '0'
    outputs = 'csv console'
  [../]
  [./average_fission_heat]
    type = AverageFissionHeat
    nt_scale = ${nt_scale}
    execute_on = 'linear nonlinear'
    outputs = 'csv console'
    block = '0'
  [../]
[]
