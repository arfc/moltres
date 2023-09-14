# Expected solution is u = cosh(x) / cosh(1)

[Mesh]
  [square]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 100
    xmin = -1
    xmax = 1
  []
[]

[Variables]
  [u]
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = u
  []
  [heat_sink]
    type = ConvectiveHeatExchanger
    variable = u
    htc = 1
    tref = 0
  []
[]

[BCs]
  [dirichlet]
    type = DirichletBC
    variable = u
    boundary = 'left right'
    value = 1
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]
