flow_velocity=1
sigma_val=6

[Mesh]
  file = two_block_mesh_mimic_msr.msh
[]

[Kernels]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
  [./source]
    type = UserForcingFunction
    variable = u
    function = 'forcing_func'
    block = 'fuel'
  [../]
  [./convection]
    type = ConservativeAdvection
    variable = u
    velocity = '0 ${flow_velocity} 0'
    block = 'fuel'
  [../]
  [./diffusion]
    type = MatDiffusion
    variable = u
    prop_name = 'k'
  [../]
[]

[Problem]
  coord_type = RZ
[../]

[DGKernels]
  [./convection]
    type = DGConvection
    variable = u
    velocity = '0 ${flow_velocity} 0'
    block = 'fuel'
  [../]
  [./diffusion]
    type = DGDiffusion
    variable = u
    diff = 'k'
    sigma = ${sigma_val}
    epsilon = -1
  [../]
[]

[BCs]
  [./advection]
    type = OutflowBC
    boundary = 'fuel_top'
    variable = u
    velocity = '0 ${flow_velocity} 0'
  [../]
  [./diffusion_left]
    type = DGFunctionDiffusionDirichletBC
    boundary = 'fuel_bottom graphite_bottom'
    variable = u
    sigma = ${sigma_val}
    epsilon = -1
    function = 'boundary_left_func'
    diff = 'k'
  [../]
[]

[Variables]
  [./u]
    family = L2_LAGRANGE
    order = FIRST
  [../]
[]

[Materials]
  [./fuel]
    block = 'fuel'
    type = GenericConstantMaterial
    prop_names = 'k'
    prop_values = '1'
  [../]
  [./moder]
    block = 'moder'
    type = GenericConstantMaterial
    prop_names = 'k'
    prop_values = '2'
  [../]
[]

[Executioner]
  # type = Steady
  type = Transient
  end_time = 10

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-12
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -ksp_type'
  petsc_options_value = 'lu preonly'

  nl_max_its = 40

  dtmin = 1e-5
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
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

[Functions]
  [./forcing_func]
    type = ParsedFunction
    value = '1'
  [../]
  [./boundary_left_func]
    type = ParsedFunction
    value = '0'
  [../]
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
