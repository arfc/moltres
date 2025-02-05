[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  temperature = temp
  sss2_input = true
  account_delayed = true
[]

[Mesh]
  coord_type = RZ
  [mesh]
    type = FileMeshGenerator
    file = '../eigenvalue/mesh.e'
  []
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
  [temp]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1000
  []
[]

[Precursors]
  [pres]
    var_name_base = pre
    family = MONOMIAL
    order = CONSTANT
    block = 0
    outlet_boundaries = 'fuel_top'
    velocity_type = constant
    u_def = 0
    v_def = 18.085
    w_def = 0
    nt_exp_form = false
    loop_precursors = false
    transient = true
  []
[]

[Kernels]
  #---------------------------------------------------------------------
  # Group 1 Neutronics
  #---------------------------------------------------------------------
  [time_group1]
    type = NtTimeDerivative
    variable = group1
    group_number = 1
  []
  [sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  []
  [diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  []
  [inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  []
  [fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    block = '0'
  []
  [delayed_group1]
    type = DelayedNeutronSource
    variable = group1
    block = '0'
    group_number = 1
  []

  #---------------------------------------------------------------------
  # Group 2 Neutronics
  #---------------------------------------------------------------------
  [time_group2]
    type = NtTimeDerivative
    variable = group2
    group_number = 2
  []
  [sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  []
  [diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  []
  [fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
    block = '0'
  []
  [inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  []

  #---------------------------------------------------------------------
  # Temperature
  #---------------------------------------------------------------------
  [temp_time_derivative]
    type = INSTemperatureTimeDerivative
    variable = temp
  []
  [temp_advection_fuel]
    type = ConservativeTemperatureAdvection
    variable = temp
    velocity = '0 18.085 0'
    block = '0'
  []
  [temp_diffusion]
    type = MatDiffusion
    variable = temp
    diffusivity = 'k'
  []
  [temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    block = '0'
  []
[]

[BCs]
  [vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top mod_bottom mod_top right'
    variable = group1
  []
  [vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top mod_bottom mod_top right'
    variable = group2
  []
  [temp_inlet_bc]
    type = FunctionDirichletBC
    variable = temp
    boundary = 'fuel_bottom mod_bottom right'
    function = 'temp_bc_func'
  []
  [temp_outlet_bc]
    type = TemperatureOutflowBC
    variable = temp
    boundary = 'fuel_top'
    velocity = '0 18.085 0'
  []
[]

[ICs]
  [group1_ic]
    type = FunctionIC
    variable = group1
    function = ic_func
  []
  [group2_ic]
    type = FunctionIC
    variable = group2
    function = ic_func
  []
[]

[Functions]
  [temp_bc_func]
    type = ParsedFunction
    expression = '1000 - (1000-965) * tanh(t/1)'
  []
  [dt_func]
    type = ParsedFunction
    expression = 'if(t<50, if(t<30, .4, 5), 10)'
  []
  [ic_func]
    type = ParsedFunction
    expression = '1e5 * (-x^2+70^2) * (-y * (y-150))'
  []
[]

[Materials]
  [fuel]
    type = MoltresJsonMaterial
    block = '0'
    base_file = '../eigenvalue/xsdata.json'
    material_key = 'fuel'
    interp_type = LINEAR
    prop_names = 'rho k cp'
    prop_values = '2.146e-3 .0553 1967'
  []
  [graphite]
    type = MoltresJsonMaterial
    block = '1'
    base_file = '../eigenvalue/xsdata.json'
    material_key = 'graphite'
    interp_type = LINEAR
    prop_names = 'rho k cp'
    prop_values = '1.86e-3 .312 1760'
  []
[]

[Executioner]
  type = Transient
  end_time = 200

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-1

  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = 0.1
  scaling_group_variables = 'group1 group2; pre1 pre2 pre3 pre4 pre5 pre6; temp'

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       NONZERO               superlu_dist'

  line_search = none

  dtmin = 1e-3
  dtmax = 10
  [TimeStepper]
    type = FunctionDT
    function = dt_func
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  []
  [powernorm]
    type = ElmIntegTotFissHeatPostprocessor
    execute_on = linear
  []
  [average_temp]
    type = ElementAverageValue
    variable = temp
    execute_on = linear
  []
  [inlet_temp]
    type = SideAverageValue
    variable = temp
    boundary = fuel_bottom
  []
  [outlet_temp]
    type = SideAverageValue
    variable = temp
    boundary = fuel_top
  []
[]

[VectorPostprocessors]
  [centerline_flux]
    type = LineValueSampler
    variable = 'group1 group2'
    start_point = '0 0 0'
    end_point = '0 150 0'
    num_points = 151
    sort_by = y
    execute_on = FINAL
  []
  [midplane_flux]
    type = LineValueSampler
    variable = 'group1 group2'
    start_point = '0 75 0'
    end_point = '69.375 75 0'
    num_points = 100
    sort_by = x
    execute_on = FINAL
  []
[]

[Outputs]
  perf_graph = true
  [exodus]
    type = Exodus
  []
  [csv]
    type = CSV
    execute_on = FINAL
  []
[]

[Debug]
[]
