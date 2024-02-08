Re = 3.6e4
density = 1      # kg cm-3
D = 1
inlet = 1
viscosity = '${fparse density * inlet * D / Re}'    # dynamic viscosity, mu = nu * rho, kg cm-1 s-1
alpha = 0.33333           # INS SUPG and PSPG stabilization parameter

[GlobalParams]
  integrate_p_by_parts = false
  viscous_form = 'traction'
  pressure = p
  velocity = vel
[]

[Mesh]
  [block]
    type = CartesianMeshGenerator
    dim = 2
    dx = '4  0.8 0.4 1.6 0.4 6.8 4  2  2  2.5 3  3.5 25'
    ix = '64 16 10 50  10 136 64 25 20 20  20 20  125'
    dy = '0.025 0.025 0.9 0.025 0.025 0.025 0.025 0.2 1.25 1  2.5 1.5 1.25 0.2 0.025 0.025'
    iy = '16   8    36 5    10   20   5    10 25   20 5   30  25   10 5    20'
  []
  [rename]
    type = SubdomainBoundingBoxGenerator
    input = block
    bottom_left = '0 0 0'
    top_right = '6 1 0'
    block_id = 1
  []
  [delete]
    type = BlockDeletionGenerator
    input = rename
    block = 1
    new_boundary = 'step'
  []
  [bottom]
    type = SideSetsFromNormalsGenerator
    input = delete
    normals = '0 -1 0'
    new_boundary = 'bottom'
    fixed_normal = true
    replace = true
  []
  [transform]
    type = TransformGenerator
    input = bottom
    transform = TRANSLATE
    vector_value = '104 0 0'
  []
  [corner_node]
    type = ExtraNodesetGenerator
    input = transform
    new_boundary = 'pinned_node'
    coord = '160 0 0'
  []
[]

[Problem]
  type = FEProblem
[]

[Variables]
  [vel]
    family = LAGRANGE_VEC
    order = FIRST
  []
  [p]
  []
  [mu_tilde]
    initial_condition = '${fparse viscosity * 3}'
  []
[]

[AuxVariables]
  [mu_turbulence]
  []
  [wall_dist]
  []
  [velx]
  []
  [vely]
  []
  [wall_shear_stress]
    family = MONOMIAL
    order = CONSTANT
  []
  [turbulent_stress]
  []
[]

[Kernels]
  [mass]
    type = INSADMass
    variable = p
  []
  [mass_pspg]
    type = INSADMassPSPG
    variable = p
  []
  [momentum_advection]
    type = INSADMomentumAdvection
    variable = vel
  []
  [momentum_viscous]
    type = INSADMomentumViscous
    variable = vel
  []
  [momentum_turbulent_viscous]
    type = INSADMomentumTurbulentViscous
    variable = vel
    mu_tilde = mu_tilde
  []
  [momentum_pressure]
    type = INSADMomentumPressure
    variable = vel
  []
  [momentum_supg]
    type = INSADMomentumSUPG
    variable = vel
  []
  [momentum_time]
    type = INSADMomentumTimeDerivative
    variable = vel
  []

  [mu_tilde_time]
    type = SATimeDerivative
    variable = mu_tilde
  []
  [mu_tilde_space]
    type = SATurbulentViscosity
    variable = mu_tilde
  []
  [mu_tilde_supg]
    type = SATurbulentViscositySUPG
    variable = mu_tilde
  []
[]

[AuxKernels]
  [mu_space]
    type = SATurbulentViscosityAux
    variable = mu_turbulence
    mu_tilde = mu_tilde
  []
  [wall_distance]
    type = WallDistanceAux
    variable = wall_dist
    walls = 'top bottom step'
    execute_on = initial
  []
  [velx]
    type = VectorVariableComponentAux
    variable = velx
    vector_variable = vel
    component = x
  []
  [vely]
    type = VectorVariableComponentAux
    variable = vely
    vector_variable = vel
    component = y
  []
  [wall_shear_stress]
    type = WallShearStressAux
    variable = wall_shear_stress
    boundary = 'top bottom'
  []
  [turbulent_stress]
    type = TurbulentStressAux
    variable = turbulent_stress
    mu_tilde = mu_tilde
  []
[]

[UserObjects]
  [inlet]
    type = SolutionUserObject
    mesh = bfs_flow_upstream_exodus.e
    system_variables = 'velx vely mu_tilde'
    timestep = LATEST
    execute_on = initial
  []
[]

[Functions]
  [xfunc]
    type = ParsedFunction
    expression = '${fparse inlet}'
  []
  [velx_bc]
    type = SolutionFunction
    solution = inlet
    from_variable = velx
  []
  [vely_bc]
    type = SolutionFunction
    solution = inlet
    from_variable = vely
  []
  [mu_tilde_bc]
    type = SolutionFunction
    solution = inlet
    from_variable = mu_tilde
  []
[]

[ICs]
  [velocity]
    type = VectorFunctionIC
    function_x = xfunc
    function_y = 0
    variable = vel
  []
[]

[BCs]
  [no_slip]
    type = ADVectorFunctionDirichletBC
    variable = vel
    boundary = 'top bottom step'
    function_x = 0
    function_y = 0
  []
  [inlet]
    type = ADVectorFunctionDirichletBC
    variable = vel
    boundary = 'left'
    function_x = velx_bc
    function_y = vely_bc
  []
  [outlet]
    type = INSADMomentumTurbulentNoBCBC
    variable = vel
    boundary = 'right'
    mu_tilde = mu_tilde
  []
  [outlet_supg]
    type = INSADMomentumSUPGBC
    variable = vel
    boundary = 'right'
  []
  [outlet_pspg]
    type = INSADMassPSPGBC
    variable = p
    boundary = 'right'
  []

  [pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  []

  [mu_inlet]
    type = FunctionDirichletBC
    variable = mu_tilde
    boundary = 'left'
    function = mu_tilde_bc
  []
  [mu_wall]
    type = DirichletBC
    variable = mu_tilde
    boundary = 'top bottom step'
    value = 0
  []
  [mu_outlet]
    type = SATurbulentViscosityNoBCBC
    variable = mu_tilde
    boundary = 'right'
  []
  [mu_outlet_supg]
    type = SATurbulentViscositySUPGBC
    variable = mu_tilde
    boundary = 'right'
  []
[]

[Materials]
  [ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'rho mu'
    prop_values = '${density} ${viscosity}'
  []
  [sa_mat]
    type = SATauMaterial
    alpha = ${alpha}
    mu_tilde = mu_tilde
    wall_distance_var = wall_dist
  []
[]

[VectorPostprocessors]
  [vel1]
    type = LineValueSampler
    variable = 'velx mu_turbulence'
    start_point = '106 1 0'
    end_point = '106 9 0'
    num_points = 25601
    sort_by = y
    execute_on = final
  []
  [vel2]
    type = LineValueSampler
    variable = 'velx mu_turbulence'
    start_point = '111 0 0'
    end_point = '111 9 0'
    num_points = 28801
    sort_by = y
    execute_on = final
  []
  [vel3]
    type = LineValueSampler
    variable = 'velx mu_turbulence'
    start_point = '114 0 0'
    end_point = '114 9 0'
    num_points = 28801
    sort_by = y
    execute_on = final
  []
  [vel4]
    type = LineValueSampler
    variable = 'velx mu_turbulence'
    start_point = '116 0 0'
    end_point = '116 9 0'
    num_points = 28801
    sort_by = y
    execute_on = final
  []
  [vel5]
    type = LineValueSampler
    variable = 'velx mu_turbulence'
    start_point = '120 0 0'
    end_point = '120 9 0'
    num_points = 28801
    sort_by = y
    execute_on = final
  []
  [upstream]
    type = LineValueSampler
    variable = 'velx p'
    start_point = '104 1.00125 0'
    end_point = '110 1.00125 0'
    num_points = 7201
    sort_by = x
    execute_on = final
  []
  [downstream]
    type = LineValueSampler
    variable = 'velx p'
    start_point = '110 0.0015625 0'
    end_point = '160 0.0015625 0'
    num_points = 60001
    sort_by = x
    execute_on = final
  []
[]

[Executioner]
  type = Transient

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       NONZERO               superlu_dist'
  line_search = none

  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = .1
  scaling_group_variables = 'vel; p; mu_tilde'
  off_diagonals_in_auto_scaling = true

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-12
  nl_max_its = 10
  l_max_its = 200

  steady_state_detection = true
  steady_state_tolerance = 1e-6
  dtmin = 1e-2
  dtmax = 10
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-1
    cutback_factor = 0.9
    growth_factor = 1.1
    optimal_iterations = 6
    iteration_window = 1
    linear_iteration_ratio = 1000
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Outputs]
  [exodus]
    type = Exodus
  []
  [csv]
    type = CSV
    execute_on = final
  []
[]

[Debug]
  show_var_residual_norms = true
[]
