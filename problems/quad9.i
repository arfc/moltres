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
    value = '-2'
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    value = 0
    boundary = 'left'
    variable = u
  [../]
  [./right]
    type = DirichletBC
    value = 1
    boundary = 'right'
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
