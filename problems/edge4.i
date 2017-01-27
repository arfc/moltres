[Mesh]
  type = GeneratedMesh
  nx = 10
  elem_type = EDGE3
  dim = 1
[]

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  [./diri_left]
    type = DirichletBC
    boundary = 'left'
    value = 1
    variable = u
  [../]
  [./diri_right]
    type = DirichletBC
    boundary = 'right'
    value = 0
    variable = u
  [../]
[]

[Variables]
  [./u]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
