[Mesh]
  file = 'cubicmesh.msh'
[]

[Variables]
  [./diffused]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = diffused
  [../]
[]

[BCs]

  [./top]
    type = DirichletBC
    variable = diffused
    boundary = 'top'
    value = 2
  [../]

  [./bottom]
    type = DirichletBC
    variable = diffused
    boundary = 'bottom'
    value = 3
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
