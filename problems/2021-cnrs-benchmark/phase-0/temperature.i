# Step 0.3: Temperature input file
# Requires neutron fluxes from Step 0.2

density = .002 # kg cm-3
cp = 3075 # J kg-1 K-1, 6.15 / 2.0e-3
k = .005 # W cm-1 K-1
gamma = 1 # W cm-3 K-1, Volumetric heat transfer coefficient
viscosity = .5 # dynamic viscosity, mu = nu * rho, kg cm-1 s-1
alpha = 1 # INS SUPG and PSPG stabilization parameter

[GlobalParams]
  num_groups = 6
  num_precursor_groups = 8
  group_fluxes = 'group1 group2 group3 group4 group5 group6'
  sss2_input = true
[]

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
  [pin]
    type = ExtraNodesetGenerator
    input = square
    new_boundary = 'pinned_node'
    coord = '200 200'
  []
[]

[Problem]
  type = FEProblem
[]

[Variables]
  [temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 900
  []
[]

[AuxVariables]
  [group1]
    family = LAGRANGE
    order = FIRST
  []
  [group2]
    family = LAGRANGE
    order = FIRST
  []
  [group3]
    family = LAGRANGE
    order = FIRST
  []
  [group4]
    family = LAGRANGE
    order = FIRST
  []
  [group5]
    family = LAGRANGE
    order = FIRST
  []
  [group6]
    family = LAGRANGE
    order = FIRST
  []
  [heat]
    family = MONOMIAL
    order = FIRST
  []
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
[]

[UserObjects]
  [fluxes]
    type = SolutionUserObject
    mesh = './nts_exodus.e'
    system_variables = 'group1 group2 group3 group4 group5 group6'
    timestep = LATEST
    execute_on = INITIAL
  []
  [velocities]
    type = SolutionUserObject
    mesh = 'vel-field_exodus.e'
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
    alpha = ${gamma}
    T_ambient = 900
  []
[]

[AuxKernels]
  [group1]
    type = SolutionAux
    variable = group1
    from_variable = group1
    solution = fluxes
  []
  [group2]
    type = SolutionAux
    variable = group2
    from_variable = group2
    solution = fluxes
  []
  [group3]
    type = SolutionAux
    variable = group3
    from_variable = group3
    solution = fluxes
  []
  [group4]
    type = SolutionAux
    variable = group4
    from_variable = group4
    solution = fluxes
  []
  [group5]
    type = SolutionAux
    variable = group5
    from_variable = group5
    solution = fluxes
  []
  [group6]
    type = SolutionAux
    variable = group6
    from_variable = group6
    solution = fluxes
  []
  [heat_source]
    type = FissionHeatSourceAux
    variable = heat
    tot_fission_heat = 1e7
    power = 1e7
  []
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
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'linear'
    prop_names = 'k rho cp'
    prop_values = '${k} ${density} ${cp}'
    temperature = 900
  []
  [ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'ad_k ad_rho ad_cp ad_mu'
    prop_values = '${k} ${density} ${cp} ${viscosity}'
  []
  [ins_mat]
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

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       NONZERO               superlu_dist'
  line_search = 'none'
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[VectorPostprocessors]
  [aa]
    type = LineValueSampler
    variable = 'temp'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = TIMESTEP_END
    outputs = 'csv'
  []
  [bb]
    type = LineValueSampler
    variable = 'temp'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = TIMESTEP_END
    outputs = 'csv'
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
    execute_on = 'final'
  []
[]

[Debug]
  show_var_residual_norms = true
[]
