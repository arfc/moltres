# Model emulating Cammi et. al.

[Variables]
  [./u]
    family = LAGRANGE
    order = SECOND
  [../]
  [./v]
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
  [./v_in]
    type = DirichletBC
    boundary = fuel_inlet
    variable = v
    value = 1.47 # m/s
  [./u_in]
    type = DirichletBC
    boundary = fuel_inlet
    variable = u
    value = 0
  [../]
[]
  