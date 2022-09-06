[GlobalParams]
  num_groups = 2
[]

[Mesh]
  file = '/home/lindsayad/gdrive/gmsh-scripts/msr-small.msh'
[]

[Variables]
  [group1]
    order = FIRST
    family = LAGRANGE
  []
  [group2]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  []
  [diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  []
  [sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  []
  [sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  []
  [inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
  []
  [inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
  []

  [fission_source_group1]
    type = CoupledFissionEigenKernel
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
  []
  [fission_source_group2]
    type = CoupledFissionEigenKernel
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '/home/lindsayad/serpent/core/examples/serpent-input/msre/msr2g_enrU_mod_953_fuel_interp_'
    num_groups = 2
  []
  [moder]
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '/home/lindsayad/serpent/core/examples/serpent-input/msre/msr2g_enrU_fuel_922_mod_interp_'
    num_groups = 2
  []
[]

[Executioner]
  type = NonlinearEigen

  bx_norm = 'bnorm'

  free_power_iterations = 2
  source_abs_tol = 1e-50
  source_rel_tol = 1e-8
  k0 = 1.0
  output_after_power_iterations = true

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason'
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    group_fluxes = 'group1 group2'
    execute_on = linear
  []
  [group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  []
  [group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  []
  [group1max]
    type = NodalExtremeValue
    value_type = max
    variable = group1
    execute_on = timestep_end
  []
  [group2max]
    type = NodalExtremeValue
    value_type = max
    variable = group2
    execute_on = timestep_end
  []
[]

[Outputs]
  [out]
    type = Exodus
    execute_on = 'timestep_end'
  []
[]
