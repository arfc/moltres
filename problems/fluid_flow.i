# Model emulating Cammi et. al.

[Variables]
  [./uz]
    family = LAGRANGE
    order = SECOND
  [../]
  [./ur]
    family = LAGRANGE
    order = SECOND
  [../]
  [./p]
    family = LAGRANGE
    order = SECOND
  [../]
  [./k]
    family = LAGRANGE
    order = SECOND
  [../]
  [./eps]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[BCs]
  [./eps_in]
    type = DirichletBC
    boundary = fuel_inlet
    variable = eps
    value = .033 # m^2/s^2
  [./k_in]
    type = DirichletBC
    boundary = fuel_inlet
    variable = k
    value = .007 # m^2/s^2
  [./uz_in]
    type = DirichletBC
    boundary = fuel_inlet
    variable = uz
    value = 1.47 # m/s
  [./ur_in]
    type = DirichletBC
    boundary = fuel_inlet
    variable = ur
    value = 0
  [../]
[]
  