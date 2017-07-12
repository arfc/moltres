base_height=136
scale=.99
height=${* ${base_height} ${scale}}

[Mesh]
  file = converged_3d_steady_state.e
[]

[MeshModifiers]
  # [./scale]
  #   type = Transform
  #   transform = SCALE
  #   vector_value = '1 1 ${scale}'
  # [../]
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
[]

[AuxVariables]
  [./group1]
  [../]
  [./group2]
  [../]
  [./pre1]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre2]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre3]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre4]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre5]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre6]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./temp]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[AuxKernels]
  [./group1]
    type = SolutionAux
    solution = soln
    variable = group1
    from_variable = group1
  [../]
  [./group2]
    type = SolutionAux
    solution = soln
    variable = group2
    from_variable = group2
  [../]
  [./pre1]
    type = SolutionAux
    solution = soln
    variable = pre1
    from_variable = pre1
  [../]
  [./pre2]
    type = SolutionAux
    solution = soln
    variable = pre2
    from_variable = pre2
  [../]
  [./pre3]
    type = SolutionAux
    solution = soln
    variable = pre3
    from_variable = pre3
  [../]
  [./pre4]
    type = SolutionAux
    solution = soln
    variable = pre4
    from_variable = pre4
  [../]
  [./pre5]
    type = SolutionAux
    solution = soln
    variable = pre5
    from_variable = pre5
  [../]
  [./pre6]
    type = SolutionAux
    solution = soln
    variable = pre6
    from_variable = pre6
  [../]
  [./temp]
    type = SolutionAux
    solution = soln
    variable = temp
    from_variable = temp
  [../]
[]

[UserObjects]
  [./soln]
    type = SolutionUserObject
    mesh = converged_3d_steady_state.e
    system_variables = 'group1 group2 pre1 pre2 pre3 pre4 pre5 pre6 temp'
    timestep = LATEST
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
