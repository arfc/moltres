[GlobalParams]
  use_exp_form = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
  []
[]

[Variables]
  [flux]
  []
[]

[Kernels]
  [diffusion]
    type = GroupDiffusion
    variable = flux
    group_number = 1
  []
[]

[BCs]
  [dirichlet]
    type = DirichletBC
    variable = flux
    boundary = 'left'
    value = 1.002
  []
  [vacuum]
    type = VacuumConcBC
    variable = flux
    boundary = 'right'
    vacuum_bc_type = marshak
  []
[]

[Materials]
  [mat]
    type = MoltresJsonMaterial
    base_file = 'xsdata.json'
    material_key = 0
    interp_type = none
    num_groups = 1
    num_precursor_groups = 1
  []
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
[]

[Postprocessors]
  [boundary_value]
    type = SideAverageValue
    variable = flux
    boundary = 'right'
  []
[]

[Outputs]
  [out]
    type = Exodus
  []
[]
