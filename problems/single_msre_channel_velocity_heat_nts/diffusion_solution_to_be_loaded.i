[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = -0.25
  ymin = -0.25
  xmax = 0.25
  ymax = .25
  # xmin = -0.5
  # ymin = -0.5
  # xmax = 0.5
  # ymax = .5
  nx = 20
  ny = 20
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    boundary = left
    variable = u
    value = 1
  [../]
  [./right]
    type = DirichletBC
    boundary = right
    variable = u
    value = 0
  [../]
[]


[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
  file_base = solution_out
[]
