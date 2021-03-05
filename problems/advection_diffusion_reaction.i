mu=1e-1
velocity=1

[GlobalParams]
  u = ${velocity}
  p = 0
  tau_type = 'mod'
  transient_term = true
[]

[Mesh]
  type = GeneratedMesh
  nx = 100
  dim = 1
[]

[Kernels]
  # [./source]
  #   type = UserForcingFunction
  #   variable = u
  #   function = 'forcing_func'
  # [../]
  [./convection]
    type = ConservativeAdvection
    variable = u
    velocity = '${velocity} 0 0'
  [../]
  [./advection_supg]
    type = AdvectionSUPG
    variable = u
  [../]
  [./diffusion]
    type = MatDiffusion
    variable = u
    diffusivity = 'mu'
  [../]
  # [./time]
  #   type = TimeDerivative
  #   variable = u
  # [../]
[]

[BCs]
  # [./outflow]
  #   type = OutflowBC
  #   boundary = 'left right'
  #   variable = u
  #   velocity = '1 0 0'
  # [../]
  [./left]
    type = DirichletBC
    boundary = left
    variable = u
    value = 1
  [../]
[]

[Problem]
[]

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
    prop_names = 'mu rho'
    prop_values = '${mu} 1'
  [../]
[]

[Executioner]
  type = Steady
  # type = Transient
  # num_steps = 1000000
  # trans_ss_check = true
  # ss_check_tol = 1e-6
  # nl_abs_tol = 1e-7
  # [./TimeStepper]
  #   dt = .01
  #   type = IterationAdaptiveDT
  #   cutback_factor = 0.4
  #   growth_factor = 1.2
  #   optimal_iterations = 5
  # [../]

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  solve_type = 'NEWTON'
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
  perf_graph = true
  print_linear_residuals = true
  [./out]
    type = Exodus
    execute_on = 'timestep_end initial'
    # execute_on = 'final'
  [../]
[]
