k_coeff = 1

[Mesh]
  type = GeneratedMesh
  nx = 1000
  dim = 1
[]

[Kernels]
  # [./time_derivative]
  #   type = TimeDerivative
  #   variable = u
  # [../]
  [./source]
    type = UserForcingFunction
    variable = u
    function = 'forcing_func'
  [../]
  [./convection]
    type = ConservativeAdvection
    variable = u
    velocity = '1 0 0'
  [../]
  [./diffusion]
    type = MatDiffusion
    variable = u
    prop_name = 'k'
  [../]
[]

[BCs]
  [./advection]
    type = DGConvectionOutflow
    boundary = 'right'
    variable = u
    velocity = '1 0 0'
  [../]
  [./dirichlet_left]
    type = DirichletBC
    value = 0
    boundary = 'left'
    variable = u
  [../]
[]

[Problem]
[../]

[Variables]
  [./u]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Materials]
  [./test]
    block = 0
    type = GenericConstantMaterial
    prop_names = 'k'
    prop_values = ${k_coeff}
  [../]
[]

[Executioner]
  type = Steady
  # type = Transient
  # start_time = 0.0
  # dtmin = 1e-6
  # end_time = 100
  # [./TimeStepper]
  #   type = IterationAdaptiveDT
  #   cutback_factor = 0.4
  #   dt = 1e-4
  #   growth_factor = 1.2
  #   optimal_iterations = 30
  # [../]
  # [./TimeIntegrator]
  #   type = ExplicitMidpoint
  # [../]
  # petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu preonly 1e-3'


  solve_type = 'NEWTON'
  nl_rel_tol = 1e-2
  nl_abs_tol = 1e-10
  nl_max_its = 50
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
    execute_on = 'timestep_end initial'
  [../]
[]
