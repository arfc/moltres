[Mesh]
  [./square]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 100
    xmin = -1
    xmax = 1
  [../]
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./time_derivative]
    type = TimeDerivative
    variable = u
  [../]
  [./diffusion]
    type = Diffusion
    variable = u
  [../]
  [./heat_sink]
    type = ConvectiveHeatExchanger
    variable = u
    htc = 1
    tref = 0
  [../]
[]

[BCs]
  [./dirichlet]
    type = DirichletBC
    variable = u
    boundary = 'left right'
    value = 1
  [../]
[]

[ICs]
  [./u_ic]
    type = FunctionIC
    variable = u
    function = 'ic_func'
  [../]
[]

[Controls]
  [./htc_control]
    type = RealFunctionControl
    parameter = '*/*/htc'
    function = 'htc_func'
  [../]
[]

[Functions]
  [./htc_func]
    type = ParsedFunction
    value = 'exp(-t)'
  [../]
  [./ic_func]
    type = ParsedFunction
    value = 'cosh(x) / cosh(1)'
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.1
  end_time = 10
[]

[Outputs]
  exodus = true
[]
