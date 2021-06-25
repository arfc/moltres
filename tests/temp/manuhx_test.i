[Mesh]
  [./square]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmin = 0
    xmax = 1
    ymin = 0
    ymax = 1
  [../]
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = u
  [../]
  [./heat_sink]
    type = ManuHX
    variable = u
    htc = 19.7392
    tref = 0
  [../]
[]

[BCs]
  [./dirichlet]
    type = DirichletBC
    variable = u
    boundary = 'left right bottom top'
    value = 1
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]
