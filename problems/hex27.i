[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 1
  dim = 3
  elem_type = HEX27
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
    value = '-6'
  [../]
  [./left]
    type = ParsedFunction
    value = 'y^2 + y*z + z^2'
  [../]
  [./right]
    type = ParsedFunction
    value = 'y^2 + y*z + y + z^2 + z + 1'
  [../]
  [./top]
    type = ParsedFunction
    value = 'x^2 + x*z + x + z^2 + z + 1'
  [../]
  [./bottom]
    type = ParsedFunction
    value = 'x^2 + x*z + z^2'
  [../]
  [./back]
    type = ParsedFunction
    value = 'x^2 + x*y + y^2'
  [../]
  [./front]
    type = ParsedFunction
    value = 'x^2 + x*y + x + y^2 + y + 1'
  [../]
[]

[BCs]
  [./left]
    type = FunctionDirichletBC
    function = 'left'
    boundary = 'left'
    variable = u
  [../]
  [./right]
    type = FunctionDirichletBC
    function = 'right'
    boundary = 'right'
    variable = u
  [../]
  [./top]
    type = FunctionDirichletBC
    function = 'top'
    boundary = 'top'
    variable = u
  [../]
  [./bottom]
    type = FunctionDirichletBC
    function = 'bottom'
    boundary = 'bottom'
    variable = u
  [../]
  [./back]
    type = FunctionDirichletBC
    function = 'back'
    boundary = 'back'
    variable = u
  [../]
  [./front]
    type = FunctionDirichletBC
    function = 'front'
    boundary = 'front'
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
