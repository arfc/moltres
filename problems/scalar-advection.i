[Mesh]
  type = GeneratedMesh
  nx = 20
  dim = 1
[]

[Kernels]
  # [./diff]
  #   type = Diffusion
  #   variable = u
  # [../]
  # [./art_diff]
  #   type = ScalarAdvectionArtDiff
  #   variable = u
  #   u_def = 1
  #   scale = 5
  # [../]
  # [./advection]
  #   type = CoupledScalarAdvection
  #   variable = u
  #   u_def = 1
  # [../]
  # [./time_derivative]
  #   type = TimeDerivative
  #   variable = u
  # [../]
  [./source]
    type = UserForcingFunction
    variable = u
    function = 'forcing_func'
  [../]
[]

[DGKernels]
  [./concentration]
    type = DGConvection
    variable = u
    velocity = '1 0 0'
  [../]
[]

[BCs]
  [./concentration]
    type = DGConvectionOutflow
    boundary = 'left right'
    variable = u
    velocity = '1 0 0'
  [../]
[]

[Problem]
  # coord_type = RZ
[../]

[Variables]
  [./u]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Executioner]
  type = Transient
  # type = Steady
  # [./TimeIntegrator]
  #   type = ExplicitMidpoint
  # [../]
  solve_type = 'NEWTON'

  l_tol = 1e-4
  nl_rel_tol = 1e-20
  nl_abs_tol = 1e-8
  nl_max_its = 60

  start_time = 0.0
  num_steps = 10 # 4 | 400 for complete run
  dt = 5e-4
  dtmin = 1e-6
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Functions]
  [./forcing_func]
    type = ParsedFunction
    value = '1'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]
