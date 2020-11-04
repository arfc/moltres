[Mesh]
  [./box]
    type = GeneratedMesh
    dim = 2
    nx = 200
    ny = 10
    xmin = 0
    xmax = 2
    ymin = 0
    ymax = 1
  [../]
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
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

[AuxKernels]
  [./v_aux]
    type = FunctionAux
    variable = v
    function = parabolic_func
  [../]
[]

[BCs]
  [./u_top]
    type = DirichletBC
    variable = u
    boundary = 'top'
    value = 1
  [../]
  [./u_bottom]
    type = DirichletBC
    variable = u
    boundary = 'bottom'
    value = 0
  [../]
[]

[Functions]
  # Parabola function for v
  [./parabolic_func]
    type = ParsedFunction
    value = '-1 * (x - 0) * (x - 2)'
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
[]

[Postprocessors]
  # Test integral of u=1 across top boundary, weighted by parabola v
  # Solution = 4/3
  [./weight_parabolic]
    type = SideWeightedIntegralPostprocessor
    variable = u
    boundary = 'top'
    weight = v
    output = 'exodus console'
  [../]

  # Test integral of parabola v across top boundary, weighted by u=1
  # Solution = 4/3
  [./variable_parabolic]
    type = SideWeightedIntegralPostprocessor
    variable = v
    boundary = 'top'
    weight = u
    output = 'exodus console'
  [../]

  # Test integral of parabola v across top boundary, weighted by parabola v
  # Solution = 16/15
  [./both_parabolic]
    type = SideWeightedIntegralPostprocessor
    variable = v
    boundary = 'top'
    weight = v
    output = 'exodus console'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
