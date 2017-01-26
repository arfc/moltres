k_coeff = 1

[Mesh]
  type = GeneratedMesh
  nx = 100
  dim = 1
[]

[Kernels]
  # [./diff]
  #   type = Diffusion
  #   variable = u
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

[DGKernels]
  [./convection]
    type = DGConvection
    variable = u
    velocity = '1 0 0'
  [../]
  [./diffusion]
    type = DGMatDiffusion
    variable = u
    prop_name = 'k'
    sigma = 6
    epsilon = -1
  [../]
[]

[BCs]
  [./advection]
    type = DGConvectionOutflow
    boundary = 'left right'
    variable = u
    velocity = '1 0 0'
  [../]
  [./diffusion_left]
    type = DGFunctionMatDiffusionDirichletBC
    boundary = 'left'
    variable = u
    sigma = 6
    epsilon = -1
    function = 'boundary_left_func'
    prop_name = 'k'
  [../]
  # [./diffusion_right]
  #   type = DGFunctionMatDiffusionDirichletBC
  #   boundary = 'right'
  #   variable = u
  #   sigma = 6
  #   epsilon = -1
  #   function = 'boundary_right_func'
  #   prop_name = 'k'
  # [../]
[]

[Problem]
[../]

[Variables]
  [./u]
    family = MONOMIAL
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
  # end_time = 2
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
  nl_max_its = 50
[]

[Preconditioning]
  [./SMP]
    type = FDP
    full = true
  [../]
[]

[Functions]
  [./forcing_func]
    type = ParsedFunction
    value = '1'
  [../]
  [./boundary_left_func]
    type = ParsedFunction
    value = '0'
  [../]
  [./boundary_right_func]
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
