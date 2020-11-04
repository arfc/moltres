[Mesh]
  [./box]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 20
    ny = 20
    xmin = 0
    xmax = 1
    ymin = 0
    ymax = 1
  [../]
[]

[Variables]
  [./u]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxVariables]
  [./v]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[DGKernels]
  [./dg_advection]
    type = DGCoupledAdvection
    variable = u
    uvel = 0
    vvel = v
  [../]
[]

[AuxKernels]
  [./v_aux]
    type = FunctionAux
    variable = v
    function = vel_func
  [../]
[]

[BCs]
  [./u_inlet]
    type = PostprocessorCoupledInflowBC
    variable = u
    boundary = 'bottom'
    postprocessor = 1
    uvel = 0
    vvel = v
  [../]
  [./u_outlet]
    type = CoupledOutflowBC
    variable = u
    boundary = 'top'
    uvel = 0
    vvel = v
  [../]
[]

[Functions]
  # Parabolic, upward flow
  [./vel_func]
    type = ParsedFunction
    value = '-(x - 0) * (x - 1)'
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
