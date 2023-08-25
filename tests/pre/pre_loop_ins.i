flow_velocity=20
global_temperature=922
xmax=50
ymax=100

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  group_fluxes = 'group1 group2'
  temperature = ${global_temperature}
  sss2_input = false
  transient = true
  pspg = true
  integrate_p_by_parts = true
[]

[Mesh]
  coord_type = RZ
  [./box]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${xmax}
    ymin = 0
    ymax = ${ymax}
    nx = 20
    ny = 20
  [../]
  [./corner_node]
    type = ExtraNodesetGenerator
    input = box
    new_boundary = corner
    coord = '0 ${ymax}'
  [../]
[]

[Problem]
  kernel_coverage_check = false
[]

[Precursors]
  [./pres]
    var_name_base = pre
    outlet_boundaries = 'top'
    outlet_vel = uy
    constant_velocity_values = false
    uvel = ux
    vvel = uy
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = true
    multi_app = loopApp
    is_loopapp = false
    inlet_boundaries = 'bottom'
  [../]
[]

[Variables]
  [./ux]
    family = LAGRANGE
    order = FIRST
    initial_condition = 0
  [../]
  [./uy]
    family = LAGRANGE
    order = FIRST
    initial_condition = ${flow_velocity}
  [../]
  [./p]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[AuxVariables]
  [./group1]
    family = LAGRANGE
    order = FIRST
  [../]
  [./group2]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Kernels]
  # Use Navier-Stokes flow at steady state for quicker run
  [./mass]
    type = INSMassRZ
    variable = p
    u = ux
    v = uy
    pressure = p
  [../]
  [./ux_momentum]
    type = INSMomentumLaplaceFormRZ
    variable = ux
    u = ux
    v = uy
    pressure = p
    component = 0
  [../]
  [./uy_momentum]
    type = INSMomentumLaplaceFormRZ
    variable = uy
    u = ux
    v = uy
    pressure = p
    component = 1
  [../]
[]

[AuxKernels]
  [./group1_flux]
    type = FunctionAux
    variable = group1
    function = fluxFunc
  [../]
  [./group2_flux]
    type = FunctionAux
    variable = group2
    function = fluxFunc
  [../]
[]

[BCs]
  [./ux_no_slip]
    type = DirichletBC
    variable = ux
    boundary = 'right'
    value = 0
  [../]
  [./uy_no_slip]
    type = DirichletBC
    variable = uy
    boundary = 'right'
    value = 0
  [../]
  [./ux_inlet]
    type = DirichletBC
    variable = ux
    boundary = 'bottom'
    value = 0
  [../]
  [./uy_inlet]
    type = FunctionDirichletBC
    variable = uy
    boundary = 'bottom'
    function = velFunc
  [../]
  [./p_pin]
    type = DirichletBC
    variable = p
    boundary = 'corner'
    value = 0
  [../]
[]

[Functions]
  [./velFunc]
    type = ParsedFunction
    value = '2 * ${flow_velocity} * (1 - (x/${xmax})^2)'
  [../]
  [./fluxFunc]
    type = ParsedFunction
    value = 'cos(pi / 2 * x / ${xmax}) * sin(pi * y / ${ymax})'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    prop_names = 'rho mu'
    prop_values = '1e-3 1'
  [../]
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
  nl_max_its = 30
  l_max_its = 100
  fixed_point_max_its = 5
  fixed_point_rel_tol = 1e-6
  fixed_point_abs_tol = 1e-5
  dtmin = 1e-2
  dtmax = 4
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
    input_files = 'sub_ins.i'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
