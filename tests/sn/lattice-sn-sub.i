N = 8
L = 2
total_ords = ${fparse N*(N+2)}

[GlobalParams]
  num_groups = 8
  num_precursor_groups = 6
  group_fluxes = 'group1_diff group2_diff group3_diff group4_diff group5_diff group6_diff group7_diff group8_diff'
  group_angular_fluxes = 'psi1 psi2 psi3 psi4 psi5 psi6 psi7 psi8'
  temperature = 900
  N = ${N}
  L = ${L}
  account_delayed = false
  use_exp_form = false
  group_constants = 'TOTXS FISSXS NSF FISSE RECIPVEL CHI_T CHI_P CHI_D SPN BETA_EFF DECAY_CONSTANT DIFFCOEF'
  acceleration = true
[]

[Mesh]
  second_order = true
  [cmg]
    type = CartesianMeshGenerator
    dim = 1
    dx = '1.9375 1.125 3.875 1.125 3.875 1.125 3.875 1.125 1.9375'
    ix = '5 4 10 4 10 4 10 4 5'
    subdomain_id = '0 1 0 1 0 1 0 1 0'
  []
[]

[Variables]
  [psi1]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi2]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi3]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi4]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi5]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi6]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi7]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
  [psi8]
    order = SECOND
    family = LAGRANGE
    components = ${total_ords}
  []
[]

[AuxVariables]
  [group1]
    order = SECOND
    family = LAGRANGE
  []
  [group2]
    order = SECOND
    family = LAGRANGE
  []
  [group3]
    order = SECOND
    family = LAGRANGE
  []
  [group4]
    order = SECOND
    family = LAGRANGE
  []
  [group5]
    order = SECOND
    family = LAGRANGE
  []
  [group6]
    order = SECOND
    family = LAGRANGE
  []
  [group7]
    order = SECOND
    family = LAGRANGE
  []
  [group8]
    order = SECOND
    family = LAGRANGE
  []
  [group1_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group2_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group3_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group4_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group5_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group6_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group7_diff]
    order = SECOND
    family = LAGRANGE
  []
  [group8_diff]
    order = SECOND
    family = LAGRANGE
  []
  [drift1]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift2]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift3]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift4]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift5]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift6]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift7]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift8]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
[]

[Kernels]
  [streaming_psi1]
    type = SNStreaming
    variable = psi1
    group_number = 1
  []
  [streaming_psi2]
    type = SNStreaming
    variable = psi2
    group_number = 2
  []
  [streaming_psi3]
    type = SNStreaming
    variable = psi3
    group_number = 3
  []
  [streaming_psi4]
    type = SNStreaming
    variable = psi4
    group_number = 4
  []
  [streaming_psi5]
    type = SNStreaming
    variable = psi5
    group_number = 5
  []
  [streaming_psi6]
    type = SNStreaming
    variable = psi6
    group_number = 6
  []
  [streaming_psi7]
    type = SNStreaming
    variable = psi7
    group_number = 7
  []
  [streaming_psi8]
    type = SNStreaming
    variable = psi8
    group_number = 8
  []
  [collision_psi1]
    type = SNCollision
    variable = psi1
    group_number = 1
  []
  [collision_psi2]
    type = SNCollision
    variable = psi2
    group_number = 2
  []
  [collision_psi3]
    type = SNCollision
    variable = psi3
    group_number = 3
  []
  [collision_psi4]
    type = SNCollision
    variable = psi4
    group_number = 4
  []
  [collision_psi5]
    type = SNCollision
    variable = psi5
    group_number = 5
  []
  [collision_psi6]
    type = SNCollision
    variable = psi6
    group_number = 6
  []
  [collision_psi7]
    type = SNCollision
    variable = psi7
    group_number = 7
  []
  [collision_psi8]
    type = SNCollision
    variable = psi8
    group_number = 8
  []
  [scattering_psi1]
    type = SNScattering
    variable = psi1
    group_number = 1
  []
  [scattering_psi2]
    type = SNScattering
    variable = psi2
    group_number = 2
  []
  [scattering_psi3]
    type = SNScattering
    variable = psi3
    group_number = 3
  []
  [scattering_psi4]
    type = SNScattering
    variable = psi4
    group_number = 4
  []
  [scattering_psi5]
    type = SNScattering
    variable = psi5
    group_number = 5
  []
  [scattering_psi6]
    type = SNScattering
    variable = psi6
    group_number = 6
  []
  [scattering_psi7]
    type = SNScattering
    variable = psi7
    group_number = 7
  []
  [scattering_psi8]
    type = SNScattering
    variable = psi8
    group_number = 8
  []
  [fission_psi1]
    type = SNFission
    variable = psi1
    block = 1
    group_number = 1
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi2]
    type = SNFission
    variable = psi2
    block = 1
    group_number = 2
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi3]
    type = SNFission
    variable = psi3
    block = 1
    group_number = 3
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi4]
    type = SNFission
    variable = psi4
    block = 1
    group_number = 4
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi5]
    type = SNFission
    variable = psi5
    block = 1
    group_number = 5
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi6]
    type = SNFission
    variable = psi6
    block = 1
    group_number = 6
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi7]
    type = SNFission
    variable = psi7
    block = 1
    group_number = 7
    eigenvalue_scaling = eigenvalue
  []
  [fission_psi8]
    type = SNFission
    variable = psi8
    block = 1
    group_number = 8
    eigenvalue_scaling = eigenvalue
  []
[]

[AuxKernels]
  [scalar_group1]
    type = SNScalarFluxAux
    variable = group1
    psi = psi1
    execute_on = timestep_end
  []
  [scalar_group2]
    type = SNScalarFluxAux
    variable = group2
    psi = psi2
    execute_on = timestep_end
  []
  [scalar_goup3]
    type = SNScalarFluxAux
    variable = group3
    psi = psi3
    execute_on = timestep_end
  []
  [scalar_group4]
    type = SNScalarFluxAux
    variable = group4
    psi = psi4
    execute_on = timestep_end
  []
  [scalar_group5]
    type = SNScalarFluxAux
    variable = group5
    psi = psi5
    execute_on = timestep_end
  []
  [scalar_group6]
    type = SNScalarFluxAux
    variable = group6
    psi = psi6
    execute_on = timestep_end
  []
  [scalar_group7]
    type = SNScalarFluxAux
    variable = group7
    psi = psi7
    execute_on = timestep_end
  []
  [scalar_group8]
    type = SNScalarFluxAux
    variable = group8
    psi = psi8
    execute_on = timestep_end
  []
  [drift1]
    type = GroupDriftAux
    variable = drift1
    group_number = 1
    execute_on = timestep_end
  []
  [drift2]
    type = GroupDriftAux
    variable = drift2
    group_number = 2
    execute_on = timestep_end
  []
  [drift3]
    type = GroupDriftAux
    variable = drift3
    group_number = 3
    execute_on = timestep_end
  []
  [drift4]
    type = GroupDriftAux
    variable = drift4
    group_number = 4
    execute_on = timestep_end
  []
  [drift5]
    type = GroupDriftAux
    variable = drift5
    group_number = 5
    execute_on = timestep_end
  []
  [drift6]
    type = GroupDriftAux
    variable = drift6
    group_number = 6
    execute_on = timestep_end
  []
  [drift7]
    type = GroupDriftAux
    variable = drift7
    group_number = 7
    execute_on = timestep_end
  []
  [drift8]
    type = GroupDriftAux
    variable = drift8
    group_number = 8
    execute_on = timestep_end
  []
[]

[BCs]
  [reflecting_psi1]
    type = SNReflectingBC
    variable = psi1
    boundary = 'left right'
  []
  [reflecting_psi2]
    type = SNReflectingBC
    variable = psi2
    boundary = 'left right'
  []
  [reflecting_psi3]
    type = SNReflectingBC
    variable = psi3
    boundary = 'left right'
  []
  [reflecting_psi4]
    type = SNReflectingBC
    variable = psi4
    boundary = 'left right'
  []
  [reflecting_psi5]
    type = SNReflectingBC
    variable = psi5
    boundary = 'left right'
  []
  [reflecting_psi6]
    type = SNReflectingBC
    variable = psi6
    boundary = 'left right'
  []
  [reflecting_psi7]
    type = SNReflectingBC
    variable = psi7
    boundary = 'left right'
  []
  [reflecting_psi8]
    type = SNReflectingBC
    variable = psi8
    boundary = 'left right'
  []
[]

[Materials]
  [fuel]
    type = MoltresSNMaterial
    base_file = '../../property_file_dir/sn-test/lattice.json'
    material_key = 'fuel'
    interp_type = 'none'
    block = '1'
  []
  [graphite]
    type = MoltresSNMaterial
    base_file = '../../property_file_dir/sn-test/lattice.json'
    material_key = 'graphite'
    interp_type = 'none'
    block = '0'
  []
[]

[Executioner]
  type = Steady

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_hypre_type -pc_hypre_boomeramg_strong_threshold -pc_hypre_boomeramg_agg_nl -pc_hypre_boomeramg_agg_num_paths -pc_hypre_boomeramg_max_levels -pc_hypre_boomeramg_coarsen_type -pc_hypre_boomeramg_interp_type -pc_hypre_boomeramg_P_max -pc_hypre_boomeramg_truncfactor -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg 0.7 4 5 25 HMIS ext+i 2 0.3 200'
  line_search = 'none'

  nl_abs_tol = 1e-7
  nl_rel_tol = 1e-50
  l_abs_tol = 1e-8
  l_tol = 1e-50
[]

[Postprocessors]
  [eigenvalue]
    type = Receiver
    default = 1
  []
[]

[Outputs]
  perf_graph = true
[]
