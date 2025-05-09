flow_velocity = 20
global_temperature = 922

[GlobalParams]
  num_groups = 0
  num_precursor_groups = 6
  temperature = ${global_temperature}
  group_fluxes = ''
  sss2_input = false
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
  xmax = 100
  elem_type = EDGE2
[]

[Precursors]
  [core]
    var_name_base = pre
    outlet_boundaries = 'right'
    outlet_vel_func = velFunc
    velocity_type = function
    u_func = velFunc
    v_func = 0
    w_func = 0
    nt_exp_form = false
    family = MONOMIAL
    order = FIRST
    loop_precursors = true
    multi_app = loopApp
    is_loopapp = true
    inlet_boundaries = 'left'
  []
[]

[Functions]
  # 1-D flow exponentially converging in time
  [velFunc]
    type = ParsedFunction
    expression = '${flow_velocity} * (1 - exp(-t/10))'
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    num_groups = 2
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
  []
[]

[Executioner]
  type = Transient
  end_time = 400
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
  dtmax = 4
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-2
    cutback_factor = 0.4
    growth_factor = 1.2
    optimal_iterations = 20
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [out]
    type = Exodus
    execute_on = 'final'
    discontinuous = true
  []
[]

[Debug]
  show_var_residual_norms = true
[]
