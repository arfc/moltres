flow_velocity=21.7 # cm/s. See MSRE-properties.ods
global_temperature=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  group_fluxes = '1 1'
  temperature = ${global_temperature}
  sss2_input = false
  transient = true
[../]

[Mesh]
  file = '2d_lattice_structured_smaller.msh'
[../]

[Problem]
  coord_type = RZ
  kernel_coverage_check = false
[]

[Precursors]
  [./pres]
    var_name_base = pre
    outlet_boundaries = 'fuel_tops'
    u_def = 0
    v_def = ${flow_velocity}
    w_def = 0
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = true
    multi_app = loopApp
    is_loopapp = false
    inlet_boundaries = 'fuel_bottoms'
    block = 'fuel'
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
  nl_max_its = 30
  l_max_its = 100
  picard_max_its = 5
  picard_rel_tol = 1e-6
  picard_abs_tol = 1e-5
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


[Outputs]
  perf_graph = true
  print_linear_residuals = true
  csv = true
  [./out]
    type = Exodus
    execute_on = 'final'
  [../]
[]

[MultiApps]
  [./loopApp]
    type = TransientMultiApp
    app_type = MoltresApp
    execute_on = timestep_begin
    positions = '200.0 200.0 0.0'
    input_files = 'sub.i'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
