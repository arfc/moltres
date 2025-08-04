[Mesh]
  [box]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 200
    ny = 10
    xmin = 0
    xmax = 2
    ymin = 0
    ymax = 1
  []
[]

[Variables]
  [u]
    order = FIRST
    family = LAGRANGE
  []
[]

[AuxVariables]
  [v]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = u
  []
[]

[AuxKernels]
  [v_aux]
    type = FunctionAux
    variable = v
    function = parabolic_func
  []
[]

[BCs]
  [u_top]
    type = DirichletBC
    variable = u
    boundary = 'top'
    value = 1
  []
  [u_bottom]
    type = DirichletBC
    variable = u
    boundary = 'bottom'
    value = 0
  []
[]

[Functions]
  # Parabola function
  [parabolic_func]
    type = ParsedFunction
    expression = '-1 * (x - 0) * (x - 2)'
  []
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
[]

[Postprocessors]
  # Test integral of u=1 across top boundary, weighted by parabola function
  # Solution = 4/3
  [weight_parabolic]
    type = SideFunctionWeightedIntegralPostprocessor
    variable = u
    boundary = 'top'
    weight_func = parabolic_func
  []

  # Test integral of parabola v across top boundary, weighted by parabola function
  # Solution = 16/15
  [both_parabolic]
    type = SideFunctionWeightedIntegralPostprocessor
    variable = v
    boundary = 'top'
    weight_func = parabolic_func
  []
[]

[Outputs]
  [out]
    type = Exodus
  []
[]
