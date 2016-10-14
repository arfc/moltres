[Mesh]
  file = 'few-element-mesh.msh'
[]

[Problem]
[]

[Executioner]
  type = Steady
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Variables]
  [./u]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
    block = 'Domain'
  [../]
  [./foce]
    type = UserForcingFunction
    variable = u
    function = 'force'
    block = 'Domain'
  [../]
[]

[BCs]
  [./diri]
    type = VacuumBC
    boundary = 'Boundary'
    variable = u
  [../]
[]

[Functions]
  [./force]
    type = ParsedFunction
    value = '1'
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]
