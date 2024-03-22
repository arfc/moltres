Re = 4e4
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
  coord_type = 'RZ'
  [pipe]
    type = CartesianMeshGenerator
    dim = 2
    dx = '0.3 0.1 0.05 0.034 0.016'
    dy = '0.025 0.475 19.5'
    ix = '10 5 5 4 10'
    iy = '20 1 39'
  []
  [corner_node]
    type = ExtraNodesetGenerator
    input = pipe
    new_boundary = 'pinned_node'
    coord = '0.5 20'
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
    initial_condition = '${fparse viscosity * 5}'
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
    family = MONOMIAL
    order = CONSTANT
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
    walls = 'right'
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
    boundary = right
  []
  [turbulent_stress]
    type = TurbulentStressAux
    variable = turbulent_stress
    mu_tilde = mu_tilde
  []
[]

[Functions]
  [yfunc]
    type = ParsedFunction
    expression = '${fparse inlet}'
  []
  [mu_func]
    type = ParsedFunction
    expression = 'if(x=0.5, 0, ${fparse viscosity * 5})'
  []
[]

[ICs]
  [velocity]
    type = VectorFunctionIC
    function_x = 0
    function_y = yfunc
    variable = vel
  []
[]

[BCs]
  [no_slip]
    type = ADVectorFunctionDirichletBC
    variable = vel
    boundary = 'right'
    function_x = 0
    function_y = 0
  []
  [inlet]
    type = ADVectorFunctionDirichletBC
    variable = vel
    boundary = 'bottom'
    function_x = 0
    function_y = yfunc
  []
  [outlet]
    type = INSADMomentumTurbulentNoBCBC
    variable = vel
    boundary = 'top'
    mu_tilde = mu_tilde
  []
  [outlet_supg]
    type = INSADMomentumSUPGBC
    variable = vel
    boundary = 'top'
  []
  [outlet_pspg]
    type = INSADMassPSPGBC
    variable = p
    boundary = 'top'
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
    boundary = 'bottom'
    function = mu_func
  []
  [mu_wall]
    type = DirichletBC
    variable = mu_tilde
    boundary = 'right'
    value = 0
  []
  [mu_outlet]
    type = SATurbulentViscosityNoBCBC
    variable = mu_tilde
    boundary = 'top'
  []
  [mu_outlet_supg]
    type = SATurbulentViscositySUPGBC
    variable = mu_tilde
    boundary = 'top'
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

[Executioner]
  type = Transient

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       NONZERO               superlu_dist'
  line_search = none

  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = 0.1
  scaling_group_variables = 'vel; p; mu_tilde'
  off_diagonals_in_auto_scaling = true

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-14
  nl_max_its = 10

  steady_state_detection = true
  steady_state_tolerance = 1e-10
  dtmin = 1e-0
  dtmax = 10
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-0
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
    execute_on = final
  []
[]

[Debug]
  show_var_residual_norms = true
[]
