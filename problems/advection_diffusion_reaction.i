k_coeff = 1

[Mesh]
  type = GeneratedMesh
  nx = 1000
  dim = 1
[]

[Kernels]
  [./source]
    type = UserForcingFunction
    variable = u
    function = 'forcing_func'
  [../]
  [./convection]
    type = ConservativeAdvection
    variable = u
    velocity = '1 0 0'
  [../]
  [./diffusion]
    type = MatDiffusion
    variable = u
    D_name = 'k'
  [../]
[]

[BCs]
  [./outflow]
    type = OutflowBC
    boundary = 'left right'
    variable = u
    velocity = '1 0 0'
  [../]
[]

[Problem]
[]

[Variables]
  [./u]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Materials]
  [./test]
    block = 0
    type = GenericConstantMaterial
    prop_names = 'k'
    prop_values = ${k_coeff}
  [../]
[]

[Executioner]
  type = Steady
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -sub_ksp_type'
  petsc_options_value = 'asm	  lu	       preonly'

  solve_type = 'NEWTON'
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Functions]
  [./forcing_func]
    type = ParsedFunction
    value = '1'
  [../]
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  [./out]
    type = Exodus
    execute_on = 'timestep_end initial'
  [../]
[]
