[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  dim = 2
  elem_type = QUAD9
[]

[Kernels]
  [./diffus]
    type = Diffusion
    variable = u
  [../]
  [./force]
    type = UserForcingFunction
    variable = u
    function = ffn
  [../]
[]

[Functions]
  [./ffn]
    type = ParsedFunction
    value = '-4'
  [../]
  [./soln]
    type = ParsedFunction
    value = 'x^2 + y^2'
[]

[BCs]
  [./all]
    type = FunctionDirichletBC
    function = 'soln'
    boundary = 'left right top bottom'
    variable = u
  [../]
[]

[Variables]
  [./u]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[Executioner]
  solve_type = NEWTON
  type = Steady
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
