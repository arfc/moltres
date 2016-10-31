[GlobalParams]
  num_groups = 2
  temperature = temp
  group_fluxes = 'group1 group2'
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
  [./temp]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  # Neutronics
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

  # Temperature
  [./temp_cond]
    type = HeatConduction
    diffusion_coefficient_name = k
    diffusion_coefficient_dT_name = d_k_d_temp
    use_displaced_mesh = false
    variable = temp
  [../]
  [./temp_source]
    type = FissionHeatSource
    tot_fissions = tot_fissions
    # MSRE full power = 10 MW; core volume 90 ft3
    power = 7000 # Watts
    variable = temp
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '/home/lindsayad/serpent/core/examples/serpent-input/msre/msr2g_enrU_mod_953_fuel_interp_'
    num_groups = 2
    prop_names = 'k d_k_d_temp'
    prop_values = '.0123 0' # Cammi 2011 at 908 K
  [../]
  [./moder]
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '/home/lindsayad/serpent/core/examples/serpent-input/msre/msr2g_enrU_fuel_922_mod_interp_'
    num_groups = 2
    prop_names = 'k d_k_d_temp'
    prop_values = '.312 0' # Cammi 2011 at 908 K
  [../]
[]

[BCs]
  [./temp]
    boundary = boundary
    type = DirichletBC
    variable = temp
    value = 900
  [../]
  # [./temp]
  #   boundary = boundary
  #   type = VacuumBC
  #   variable = temp
  # [../]
[]

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
    type = ElmIntegTotFissNtsPostprocessor
    group_fluxes = 'group1 group2'
    execute_on = linear
  [../]
  [./tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  [../]
[]

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]
