[GlobalParams]
  num_groups = 2
[../]


[Mesh]
  file = '/home/lindsayad/gdrive/gmsh-scripts/msr-small.msh'
[../]

[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
  [../]
  [./group2]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  [../]
  [./diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  [../]
  [./sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  [../]
  [./sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  [../]
  [./inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
  [../]

  [./fission_source_group1]
    type = CoupledFissionEigenKernel
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
  [../]
  [./fission_source_group2]
    type = CoupledFissionEigenKernel
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '/home/lindsayad/serpent/core/examples/serpent-input/msre/msr2g_enrU_mod_953_fuel_interp_'
    num_groups = 2
  [../]
  [./moder]
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '/home/lindsayad/serpent/core/examples/serpent-input/msre/msr2g_enrU_fuel_922_mod_interp_'
    num_groups = 2
  [../]
[]

# [BCs]
#   [./homogeneous_group1]
#     type = DirichletBC
#     variable = group1
#     boundary = 'boundary'
#     value = 0
#   [../]
#   [./homogeneous_group2]
#     type = DirichletBC
#     variable = group2
#     boundary = 'boundary'
#     value = 0
#   [../]
# []

[Executioner]
  type = NonlinearEigen

  bx_norm = 'bnorm'

  free_power_iterations = 2
  source_abs_tol = 1e-12
  source_rel_tol = 1e-50
  k0 = 1.0
  output_after_power_iterations = true

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./bnorm]
    type = ElementIntegralTotFissionSrcPostprocessor
    group_fluxes = 'group1 group2'
    execute_on = linear
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]
