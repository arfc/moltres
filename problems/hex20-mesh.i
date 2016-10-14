[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 1
  dim = 3
  elem_type = HEX20
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
    # block = 'Domain'
  [../]
  [./foce]
    type = UserForcingFunction
    variable = u
    function = 'force'
    # block = 'Domain'
  [../]
[]

[BCs]
  [./diri]
    type = VacuumBC
    boundary = 'left right top bottom front back'
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
