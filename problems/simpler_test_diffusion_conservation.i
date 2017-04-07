[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
[]

[Variables]
  [./u]
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

[BCs]
  [./left]
    type = FunctionDirichletBC
    function = 'ffn'
    variable = u
    boundary = 'left'
  [../]
  [./right]
    type = FunctionDirichletBC
    function = 'ffn'
    variable = u
    boundary = 'right'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10
  dt = 1e-6
  solve_type = 'NEWTON'
[]

[Functions]
  [./ffn]
    type = ParsedFunction
    value = '922 + 500*tanh(t/1e-5)'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
