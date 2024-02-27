[GlobalParams]
  pressure = p
  velocity = vel
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
  []
[]

[Variables]
  [prec]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxVariables]
  [mu_tilde]
    initial_condition = 1
  []
  [vel]
    family = LAGRANGE_VEC
    order = FIRST
  []
  [p]
  []
  [wall_dist]
  []
[]

[DGKernels]
  [turbulent_diffusion]
    type = DGTurbulentDiffusion
    variable = prec
    mu_tilde = mu_tilde
  []
[]

[BCs]
  [left]
    type = PenaltyDirichletBC
    variable = prec
    boundary = 'left'
    value = 1
    penalty = 1e5
  []
  [right]
    type = PenaltyDirichletBC
    variable = prec
    boundary = 'right'
    value = 0
    penalty = 1e6
  []
[]

[Materials]
  [const_mat]
    type = ADGenericConstantMaterial
    prop_names = 'rho mu'
    prop_values = '1 1'
  []
  [sa_mat]
    type = SATauMaterial
    alpha = 0.33333
    mu_tilde = mu_tilde
    wall_distance_var = wall_dist
    use_ft2_term = true
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
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
