# Step 1.2: Power coupling input file
# Requires velocity fields from Step 0.1

density = .002 # kg cm-3
cp = 3075 # J kg-1 K-1, 6.15 / 2.0e-3
k = .005 # W cm-1 K-1
gamma = 1 # W cm-3 K-1, Volumetric heat transfer coefficient
viscosity = .5 # dynamic viscosity
alpha = 1 # SUPG stabilization parameter

[Mesh]
  [square]
    type = GeneratedMeshGenerator
    dim = 2

    nx = 200
    ny = 200
    ## Use a 40-by-40 mesh instead if running on a less capable computer
    #    nx = 40
    #    ny = 40

    xmin = 0
    xmax = 200
    ymin = 0
    ymax = 200
    elem_type = QUAD4
  []
[]

[Variables]
  [temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 900
  []
[]

[AuxVariables]
  [velx]
    family = LAGRANGE
    order = FIRST
  []
  [vely]
    family = LAGRANGE
    order = FIRST
  []
  [vel]
    family = LAGRANGE_VEC
    order = FIRST
  []
  [p]
    family = LAGRANGE
    order = FIRST
  []
  [heat]
    family = MONOMIAL
    order = FIRST
    initial_condition = 250
  []
[]

[UserObjects]
  [velocities]
    type = SolutionUserObject
    mesh = '../phase-0/vel-field_exodus.e'
    system_variables = 'vel_x vel_y p'
    timestep = LATEST
    execute_on = INITIAL
  []
[]

[Kernels]
  [temp_source]
    type = INSADEnergySource
    variable = temp
    source_variable = heat
  []
  [temp_advection]
    type = INSADEnergyAdvection
    variable = temp
  []
  [temp_conduction]
    type = ADHeatConduction
    variable = temp
    thermal_conductivity = 'ad_k'
  []
  [temp_supg]
    type = INSADEnergySUPG
    variable = temp
    velocity = vel
  []
  [temp_sink]
    type = INSADEnergyAmbientConvection
    variable = temp
    # alpha is the heat transfer coefficient.
    alpha = ${gamma}
    T_ambient = 900
  []
[]

[AuxKernels]
  [ux]
    type = SolutionAux
    variable = velx
    from_variable = vel_x
    solution = velocities
  []
  [uy]
    type = SolutionAux
    variable = vely
    from_variable = vel_y
    solution = velocities
  []
  [p]
    type = SolutionAux
    variable = p
    from_variable = p
    solution = velocities
  []
[]

[Functions]
  [velxf]
    type = SolutionFunction
    from_variable = vel_x
    solution = velocities
  []
  [velyf]
    type = SolutionFunction
    from_variable = vel_y
    solution = velocities
  []
[]

[ICs]
  [vel_ic]
    type = VectorFunctionIC
    variable = vel
    function_x = velxf
    function_y = velyf
  []
[]

[Materials]
  [ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'ad_k ad_rho ad_cp ad_mu'
    prop_values = '${k} ${density} ${cp} ${viscosity}'
  []
  [ins_temp]
    type = INSADStabilized3Eqn
    k_name = 'ad_k'
    rho_name = 'ad_rho'
    cp_name = 'ad_cp'
    mu_name = 'ad_mu'
    alpha = ${alpha}
    velocity = vel
    pressure = p
    temperature = temp
  []
[]

[Executioner]
  type = Steady

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
  petsc_options_value = 'asm      lu           200                1               NONZERO'
  line_search = none

  automatic_scaling = true
  compute_scaling_once = false
  off_diagonals_in_auto_scaling = true
  resid_vs_jac_scaling_param = .1

  nl_abs_tol = 1e-10
  fixed_point_max_its = 10
[]

[VectorPostprocessors]
  [temp_aa]
    type = LineValueSampler
    variable = 'temp'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = FINAL
    outputs = 'csv'
  []
  [temp_bb]
    type = LineValueSampler
    variable = 'temp'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = FINAL
    outputs = 'csv'
  []
[]

[MultiApps]
  [ntsApp]
    type = FullSolveMultiApp
    app_type = MoltresApp
    execute_on = timestep_end
    input_files = 'power-coupling-nts.i'
  []
[]

[Transfers]
  [to_sub]
    type = MultiAppCopyTransfer
    to_multi_app = ntsApp
    source_variable = temp
    variable = temp
  []
  [from_sub]
    type = MultiAppCopyTransfer
    from_multi_app = ntsApp
    source_variable = heat
    variable = heat
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [exodus]
    type = Exodus
  []
  [csv]
    type = CSV
  []
[]
