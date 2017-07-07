global_temperature=922

[GlobalParams]
  num_groups = 1
  num_precursor_groups = 1
  use_exp_form = true
[../]

[Mesh]
  type = GeneratedMesh
  xmax = 3.141592653589793238462643383279502884197169399375105820974944592307816406286
  nx = 100
  dim = 1
[../]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'left right'
  temp_scaling = 1e0
  create_temperature_var = false
  temperature_value = ${global_temperature}
  dg_for_temperature = false
[]

[Materials]
  [./homo_reactor]
    type = GenericMoltresMaterial
    block = 0
    property_tables_root = '../property_file_dir/unity_'
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
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor -snes_test_display'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda -sub_pc_factor_shift_type -sub_pc_factor_shift_amount'
  petsc_options_value = 'asm      lu           1               preonly       1e-3                       NONZERO                   1e-10'

  # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

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
