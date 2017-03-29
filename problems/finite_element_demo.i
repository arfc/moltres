[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 5
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./force]
    type = UserForcingFunction
    variable = u
    function = 'ffn'
  [../]
[]

[BCs]
  [./bounds]
    type = FunctionDirichletBC
    function = 'soln'
    variable = u
    boundary = 'left right'
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
[]

[Functions]
  [./ffn]
    type = ParsedFunction
    value = '-e^x'
  [../]
  [./soln]
    type = ParsedFunction
    value = 'e^x'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
