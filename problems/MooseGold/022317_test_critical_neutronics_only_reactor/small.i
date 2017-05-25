reactor_height=10
global_temperature=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  use_exp_form = false
  # group_fluxes = 'group1 group2'
[../]

[Mesh]
  file = 'cylinder_small.msh'
[../]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'boundaries'
  temp_scaling = 1e0
  nt_ic_function = 'nt_ic_func'
  create_temperature_var = false
  # temperature = ${global_temperature}
  temperature_value = ${global_temperature}
  dg_for_temperature = false
[]

[Materials]
  [./homo_reactor]
    type = GenericMoltresMaterial
    block = 'reactor'
    property_tables_root = '../property_file_dir/msre_homogeneous_critical_question_mark_'
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
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'asm      lu           1               preonly       1e-3'

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
