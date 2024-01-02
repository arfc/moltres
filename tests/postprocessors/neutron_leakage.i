[GlobalParams]
  group_fluxes = 'group1 group2'
  num_groups = 2
  num_precursor_groups = 0
  use_exp_form = false
  temperature = 900
  sss2_input = true
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    ny = 2
    xmax = 1
    ymax = 1
  []
[]

[Variables]
  [group1]
  []
  [group2]
  []
[]

[Kernels]
  [group1_diffusion]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  []
  [group2_diffusion]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  []
[]

[BCs]
  [group1_left]
    type = DirichletBC
    variable = group1
    boundary = left
    value = 3
  []
  [group1_right]
    type = DirichletBC
    variable = group1
    boundary = right
    value = 2
  []
  [group2_left]
    type = DirichletBC
    variable = group2
    boundary = left
    value = 30
  []
  [group2_right]
    type = DirichletBC
    variable = group2
    boundary = right
    value = 20
  []
[]

[Materials]
  [mat]
    type = MoltresJsonMaterial
    base_file = mat.json
    material_key = 'mat1'
    interp_type = 'none'
    group_constants = 'DIFFCOEF'
  []
[]

[Executioner]
  type = Steady
[]

[Postprocessors]
  [group1_leakage]
    type = NeutronLeakage
    variable = group1
    boundary = 'right'
    group_number = 1
  []
  [group2_leakage]
    type = NeutronLeakage
    variable = group2
    boundary = 'right'
    group_number = 2
  []
  [total_leakage]
    type = TotalNeutronLeakage
    boundary = 'right'
  []
[]

[Outputs]
  csv = true
[]
