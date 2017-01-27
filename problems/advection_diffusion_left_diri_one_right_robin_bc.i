flow_velocity=100

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  xmax = 1
[]

[Variables]
  [./u]
    order = FIRST
    family = MONOMIAL
    block = 0
  [../]
[]

[Kernels]
  [./test_u]
    type = Diffusion
    variable = u
    block = 0
  [../]
  [./adv_u]
    type = ConservativeAdvection
    variable = u
    velocity = '${flow_velocity} 0 0'
    block = 0
  [../]
[]

[DGKernels]
  [./dg_advection_u]
    type = DGConvection
    variable = u
    velocity = '${flow_velocity} 0 0'
    block = 0
  [../]
  [./dg_diffusion_u]
    type = DGDiffusion
    variable = u
    sigma = 6
    epsilon = -1
    block = 0
  [../]
[]

[BCs]
  # [./left]
  #   type = InflowBC
  #   variable = u
  #   boundary = 'left'
  #   inlet_conc = 1
  #   velocity = '${flow_velocity} 0 0'
  # [../]
  [./diri_left]
    boundary = 'left'
    type = DGFunctionDiffusionDirichletBC
    variable = u
    sigma = 6
    epsilon = -1
    function = 'inlet_val'
  [../]
  [./right]
    type = RobinBC
    variable = u
    boundary = 'right'
    velocity = ${flow_velocity}
  [../]
[]

[Functions]
  [./inlet_val]
    type = ParsedFunction
    value = '1'
  [../]
[]

[Preconditioning]
  [./fdp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
  nl_abs_tol = 1e-12
  solve_type = NEWTON
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]

[Debug]
  show_var_residual_norms = true
[]
