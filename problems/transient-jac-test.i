flow_velocity=147 # Cammi 147 cm/s

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  group_fluxes = 'group1 group2'
  # MSRE full power = 10 MW; core volume 90 ft3
  # power = 200000
  temperature = temp
[../]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 1
  block_id = '0'
  block_name = 'fuel'
[../]

[MeshModifiers]
  [./bounding_box]
    type = SubdomainBoundingBox
    bottom_left = '0.5 0 0'
    top_right = '1 1 0'
    block_id = '1'
    block_name = 'moder'
  [../]
[]

[Variables]
  # [./pre1]
  # [../]
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
    scaling = 1e-3
  [../]
[]

[Kernels]
  # Neutronics
  [./time_group1]
    type = NtTimeDerivative
    group_number = 1
    variable = group1
  [../]
  [./time_group2]
    type = NtTimeDerivative
    group_number = 2
    variable = group2
  [../]
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
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
  [../]
  [./fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
  [../]

  # Temperature
  [./temp_flow_fuel]
    block = 'fuel'
    type = MatINSTemperatureRZ
    variable = temp
    rho = 'rho'
    k = 'k'
    cp = 'cp'
    uz = ${flow_velocity}
  [../]
  [./temp_flow_moder]
    block = 'moder'
    type = MatINSTemperatureRZ
    variable = temp
    rho = 'rho'
    k = 'k'
    cp = 'cp'
  [../]
  [./temp_source]
    type = TransientFissionHeatSource
    variable = temp
  [../]
[]

# Delayed neutron precursors

[PrecursorKernel]
  var_name_base = pre
  v_def = ${flow_velocity}
  # block = 'fuel'
  inlet_boundary = 'bottom'
  inlet_boundary_condition = 'NeumannBC'
  inlet_bc_value = 1.1
  outlet_boundary = 'top'
  temperature = temp
  incompressible_flow = false
  transient_simulation = true
  use_exp_form = false
  jac_test = true
  # initial_condition = -20
[]

# [AuxVariables]
#   [./pre1_lin]
#     family = MONOMIAL
#     order = CONSTANT
#   [../]

#   [./Qf]
#     family = MONOMIAL
#     order = CONSTANT
#   [../]
  # [./diffus_temp]
  #   family = MONOMIAL
  #   order = CONSTANT
  # [../]
  # [./diffus_resid]
  #   family = LAGRANGE
  #   order = FIRST
  # [../]
  # [./src_resid]
  #   family = LAGRANGE
  #   order = FIRST
  # [../]
  # [./bc_resid]
  #   family = LAGRANGE
  #   order = FIRST
  # [../]
  # [./tot_resid]
  #   family = LAGRANGE
  #   order = FIRST
  # [../]
# [../]

# [AuxKernels]
#   [./pre1_lin]
#     variable = pre1_lin
#     density_log = pre1
#     type = Density
#   [../]

#   [./Qf]
#     type = FissionHeatSourceAux
#     variable = Qf
#     tot_fissions = tot_fissions
#   [../]
  # [./diffus_temp]
  #   type = MatDiffusionAux
  #   variable = diffus_temp
  #   diffuse_var = temp
  #   prop_name = 'k'
  # [../]
# []

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = 'msr2g_enrU_mod_953_fuel_interp_'
    num_groups = 2
    num_precursor_groups = 8
    prop_names = 'k rho cp'
    prop_values = '.0123 3.327e-3 1357' # Cammi 2011 at 908 K
  [../]
  [./moder]
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = 'msr2g_enrU_fuel_922_mod_interp_'
    num_groups = 2
    num_precursor_groups = 8
    prop_names = 'k rho cp'
    prop_values = '.312 1.843e-3 1760' # Cammi 2011 at 908 K
  [../]
[]

[BCs]
  # [./temp_inlet]
  #   boundary = 'fuel_bottom graphite_bottom'
  #   type = DirichletBC
  #   variable = temp
  #   value = 900
  # [../]
  # [./temp_outlet]
  #   boundary = 'fuel_top'
  #   type = MatINSTemperatureNoBCBC
  #   variable = temp
  #   k = 'k'
  # [../]
  # [./group1_vacuum]
  #   type = VacuumBC
  #   variable = group1
  #   boundary = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  # [../]
  # [./group2_vacuum]
  #   type = VacuumBC
  #   variable = group2
  #   boundary = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  # [../]
[]

[Problem]
  type = FEProblem
  coord_type = RZ
  kernel_coverage_check = false
[../]

[Executioner]
  type = Transient
  end_time = 10

  nl_abs_tol = 1e-11

# solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor -snes_test_display'
  # This system will not converge with default preconditioning; need to use asm
  petsc_options_iname = '-snes_type'
  petsc_options_value = 'test'

  dtmin = 1e-7
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
    growth_factor = 1.2
    optimal_iterations = 20
    nl_max_its = 50
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

# [Postprocessors]
#   [./bnorm]
#     type = ElmIntegTotFissNtsPostprocessor
#     group_fluxes = 'group1 group2'
#     execute_on = linear
#   [../]
#   [./tot_fissions]
#     type = ElmIntegTotFissPostprocessor
#     execute_on = linear
#   [../]
#   [./group1norm]
#     type = ElementIntegralVariablePostprocessor
#     variable = group1
#     execute_on = linear
#   [../]
#   [./group2norm]
#     type = ElementIntegralVariablePostprocessor
#     variable = group2
#     execute_on = linear
#   [../]
#   [./group1max]
#     type = NodalMaxValue
#     variable = group1
#     execute_on = timestep_end
#   [../]
#   [./group2max]
#     type = NodalMaxValue
#     variable = group2
#     execute_on = timestep_end
#   [../]
#   [./group1diff]
#     type = ElementL2Diff
#     variable = group1
#     execute_on = 'linear timestep_end'
#     use_displaced_mesh = false
#   [../]
# []

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'initial timestep_end'
  [../]
  [./dof_map]
    type = DOFMap
  [../]

[]

[Debug]
  show_var_residual_norms = true
[]

[ICs]
  [./temp_ic]
    type = RandomIC
    variable = temp
    min = 900
    max = 1400
  [../]
  [./group1_ic]
    type = RandomIC
    variable = group1
    min = 0
    max = 600
  [../]
  [./group2_ic]
    type = RandomIC
    variable = group2
    min = 0
    max = 600
  [../]
  # [./pre1_ic]
  #   type = ConstantIC
  #   variable = pre1
  #   value = -20
  # [../]
[]
