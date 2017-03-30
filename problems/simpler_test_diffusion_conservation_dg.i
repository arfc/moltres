[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 200
[]

[Variables]
  [./u]
    family = MONOMIAL
    order = FIRST
    initial_condition = 922
  [../]
[]

[Kernels]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[DGKernels]
  active = 'dg_diff'

  [./dg_diff]
    type = DGDiffusion
    variable = u
    epsilon = -1
    sigma = 6
  [../]
[]

[BCs]
  [./all]
    type = DGFunctionDiffusionDirichletBC
    variable = u
    boundary = 'left right'
    function = 'ffn'
    epsilon = -1
    sigma = 6
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10
  dt = 1e-7
  solve_type = 'NEWTON'
[]

[Functions]
  [./ffn]
    type = ParsedFunction
    value = '922 + 500*tanh(t/1e-6)'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
