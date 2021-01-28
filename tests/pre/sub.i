flow_velocity=21.7 # cm/s. See MSRE-properties.ods
global_temperature=922

[GlobalParams]
  num_groups = 0
  num_precursor_groups = 6
  temperature = ${global_temperature}
  use_exp_form = false
  group_fluxes = ''
  sss2_input = false
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 14
  xmax = 150
  elem_type = EDGE2
[../]

[Precursors]
  [./core]
    var_name_base = pre
    outlet_boundaries = 'right'
    u_def = ${flow_velocity}
    v_def = 0
    w_def = 0
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = true
    multi_app = loopApp
    is_loopapp = true
    inlet_boundaries = 'left'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
  [../]
[]

[Executioner]
  type = Transient
  end_time = 500
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu       NONZERO'
  line_search = 'none'
  nl_max_its = 20
  l_max_its = 50
  dtmin = 1e-2
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-2
    cutback_factor = 0.4
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

[Postprocessors]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./out]
    type = Exodus
    execute_on = 'final'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
