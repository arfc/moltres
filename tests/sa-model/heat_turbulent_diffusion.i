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
  [temp]
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

[Kernels]
  [turbulent_diffusion]
    type = INSADHeatTurbulentDiffusion
    variable = temp
    mu_tilde = mu_tilde
  []
[]

[BCs]
  [left]
    type = ADDirichletBC
    variable = temp
    boundary = 'left'
    value = 1
  []
  [right]
    type = ADDirichletBC
    variable = temp
    boundary = 'right'
    value = 0
  []
[]

[Materials]
  [const_mat]
    type = ADGenericConstantMaterial
    prop_names = 'cp rho mu k grad_k'
    prop_values = '1 1 1 1 0'
  []
  [sa_mat]
    type = SATauStabilized3Eqn
    alpha = 0.33333
    mu_tilde = mu_tilde
    wall_distance_var = wall_dist
    use_ft2_term = true
    temperature = temp
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
