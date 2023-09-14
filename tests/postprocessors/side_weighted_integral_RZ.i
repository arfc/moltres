[Mesh]
  coord_type = RZ
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
  # Parabola function for v
  [parabolic_func]
    type = ParsedFunction
    expression = '1 - (x / 2)^2'
  []
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
[]

[Postprocessors]
  # Test integral of u=1 across top boundary, weighted by parabola v
  # Solution = 2*pi
  [weight_parabolic]
    type = SideWeightedIntegralPostprocessor
    variable = u
    boundary = 'top'
    weight = v
  []

  # Test integral of parabola v across top boundary, weighted by u=1
  # Solution = 2*pi
  [variable_parabolic]
    type = SideWeightedIntegralPostprocessor
    variable = v
    boundary = 'top'
    weight = u
  []

  # Test integral of parabola v across top boundary, weighted by parabola v
  # Solution = 4 * pi / 3
  [both_parabolic]
    type = SideWeightedIntegralPostprocessor
    variable = v
    boundary = 'top'
    weight = v
  []
[]

[Outputs]
  [out]
    type = Exodus
  []
[]
