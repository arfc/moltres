[Mesh]
  file = 'diffusion_conservation.msh'
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
  [./diff]
    type = MatDiffusion
    variable = u
    prop_name = 'diff'
  [../]
  [./source]
    type = UserForcingFunction
    function = 'ffn'
    block = 'fuel'
    variable = u
  [../]
[]

[Materials]
  [./fuel]
    type = GenericConstantMaterial
    prop_names = 'diff'
    prop_values = '1'
    block = 'fuel'
  [../]
  [./moder]
    type = GenericConstantMaterial
    prop_names = 'diff'
    prop_values = '10'
    block = 'mod'
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    value = 0
    variable = u
    boundary = 'left'
  [../]
  [./right]
    type = DirichletBC
    value = 0
    variable = u
    boundary = 'right'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10
  dt = 1
  solve_type = 'NEWTON'
[]

[Functions]
  [./ffn]
    type = ParsedFunction
    value = '1000'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
