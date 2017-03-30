k_coeff = 1

[Mesh]
  type = GeneratedMesh
  nx = 2
  dim = 1
[]

[Kernels]
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
  petsc_options = '-snes_test_display'
  petsc_options_iname = '-snes_type'
  petsc_options_value = 'test'


  solve_type = 'NEWTON'
  nl_rel_tol = 1e-2
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
  [./boundary_left_func]
    type = ParsedFunction
    value = '0'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'timestep_end initial'
  [../]
[]

[ICs]
  [./random]
    type = RandomIC
    min = 1
    max = 2
    variable = u
  [../]
[]
