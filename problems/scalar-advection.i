[Mesh]
  type = GeneratedMesh
  nx = 10
  ny = 5
  dim = 2
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./advection]
    type = CoupledScalarAdvection
    variable = u
    v_def = 1
  [../]
  [./source]
    type = UserForcingFunction
    variable = u
    function = 'forcing_func'
  [../]
[]

[BCs]
  [./inlet]
    type = DirichletBC
    value = 0
    boundary = 'bottom'
    variable = u
  [../]
  [./out_diffusion]
    type = DiffusionNoBCBC
    variable = u
    boundary = 'top'
  [../]
  [./out_advection]
    type = CoupledScalarAdvectionNoBCBC
    variable = u
    boundary = 'top'
    v_def = 1
  [../]
[]

[Problem]
  # coord_type = RZ
[../]

[Variables]
  [./u]
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # This system will not converge with default preconditioning; need to use asm
  petsc_options_iname = '-pc_type -sub_pc_type -sub_ksp_type -pc_asm_overlap -ksp_gmres_restart'
  petsc_options_value = 'asm lu preonly 2 31'
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
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]
