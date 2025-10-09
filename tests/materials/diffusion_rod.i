[GlobalParams]
  num_groups = 8
  num_precursor_groups = 6
  group_fluxes = 'group1 group2 group3 group4 group5 group6 group7 group8'
  temperature = 900
  sss2_input = true
  account_delayed = false
  use_exp_form = false
[]

[Problem]
  type = EigenProblem
  bx_norm = bnorm
[]

[Mesh]
  [mesh]
    type = CartesianMeshGenerator
    dim = 3
    dx = 10
    dy = 10
    dz = '90 10'
    iz = '90 40'
  []
[]

[Nt]
  family = LAGRANGE
  order = FIRST
  var_name_base = group
  create_temperature_var = false
  eigen = true
[]

[Functions]
  [rod_height]
    type = ParsedFunction
    expression = '95'
  []
[]

[Materials]
  [rod_fuel]
    type = DiffusionRodMaterial
    base_file = '../../property_file_dir/sn-test/absorber-air-lattice-ref.json'
    material_key = 'ctrlrod'
    nonrod_material_key = 'fuel'
    rod_height_func = rod_height
    interp_type = 'none'
  []
[]

[Executioner]
  type = Eigenvalue
  free_power_iterations = 2

  fixed_point_abs_tol = 1e-10
  fixed_point_rel_tol = 1e-8
  fixed_point_max_its = 8
  accept_on_max_fixed_point_iteration = false

  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-50

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg 200'
  line_search = 'none'
[]

[Postprocessors]
  [bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = 'initial linear timestep_end'
  []
  [eigenvalue]
    type = VectorPostprocessorComponent
    vectorpostprocessor = eigenvalues
    vector_name = eigen_values_real
    index = 0
  []
[]

[VectorPostprocessors]
  [eigenvalues]
    type = Eigenvalues
    inverse_eigenvalue = true
    outputs = console
  []
[]

[Outputs]
  perf_graph = true
  [exodus]
    type = Exodus
    discontinuous = true
    execute_on = 'initial timestep_end failed'
  []
[]
