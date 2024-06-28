flow_velocity = 20
global_temperature = 922
xmax = 50
ymax = 100

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  group_fluxes = 'group1 group2'
  temperature = ${global_temperature}
  sss2_input = false
  transient = true
[]

[Mesh]
  coord_type = RZ
  [box]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${xmax}
    ymin = 0
    ymax = ${ymax}
    nx = 20
    ny = 20
    bias_x = 0.9
  []
  [corner_node]
    type = ExtraNodesetGenerator
    input = box
    new_boundary = corner
    coord = '0 ${ymax}'
  []
[]

[Problem]
  kernel_coverage_check = false
[]

[Precursors]
  [pres]
    var_name_base = pre
    outlet_boundaries = 'top'
    outlet_vel_func = velFunc
    velocity_type = function
    u_func = 0
    v_func = velFunc
    w_func = 0
    nt_exp_form = false
    family = MONOMIAL
    order = FIRST
    loop_precursors = true
    multi_app = loopApp
    is_loopapp = false
    inlet_boundaries = 'bottom'
    inlet_bc_penalty = 1e3
  []
[]

[AuxVariables]
  [group1]
    family = LAGRANGE
    order = FIRST
  []
  [group2]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxKernels]
  [group1_flux]
    type = FunctionAux
    variable = group1
    function = fluxFunc
  []
  [group2_flux]
    type = FunctionAux
    variable = group2
    function = fluxFunc
  []
[]

[Functions]
  # Parabolic laminar flow exponentially converging in time
  [velFunc]
    type = ParsedFunction
    expression = '2 * ${flow_velocity} * (1 - (x/${xmax})^2) * (1 - exp(-t/10))'
  []
  # Cosine-shaped flux source with extrapolations at the boundaries
  [fluxFunc]
    type = ParsedFunction
    expression = 'cos(pi / 2 * x / (${xmax}+1)) * sin(pi * (y+1) / (${ymax}+2))'
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
  []
[]

[Executioner]
  type = Transient
  end_time = 200
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu       NONZERO'
  line_search = 'none'
  nl_max_its = 30
  l_max_its = 100
  fixed_point_max_its = 5
  fixed_point_rel_tol = 1e-6
  fixed_point_abs_tol = 1e-5
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
  csv = true
  [out]
    type = Exodus
    execute_on = 'final'
    discontinuous = true
  []
[]

[MultiApps]
  [loopApp]
    type = TransientMultiApp
    app_type = MoltresApp
    execute_on = timestep_begin
    positions = '200.0 200.0 0.0'
    input_files = 'sub_func.i'
  []
[]

[Debug]
  show_var_residual_norms = true
[]
