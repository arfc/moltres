# NtAction

!alert warning
This action sets up ONLY the variables and kernels for neutron diffusion, NOT the variables and kernels for precursor tracking. 
To generate the precursor variables and kernels, see [PrecursorAction](PrecursorAction.md).

!syntax description /Nt/NtAction

## Overview

+NtAction+ greatly reduces input file length and complexity by internally generating diffusion variables and kernels.
When including only the required input parameters, this action constructs the neutron group variables and their associated kernels:
[SigmaR](SigmaR.md), [GroupDiffusion](GroupDiffusion.md), [InScatter](InScatter.md), and [CoupledFissionKernel](CoupledFissionKernel.md).
With 'account_delayed' set to +True+, [DelayedNeutronSource](DelayedNeutronSource.md) is also constructed, otherwise this kernel is not constructed and the simulation does not consider the delayed neutron precursor effects on the neutron flux distributions.

For more information regarding the use of +NtAction+ please refer to the tutorials located [here](tutorials.md), specifically the +Multiphysics Reactor Simulations+ section.

## Example Input File Syntax

An example input file without the +NtAction+, showing only the portion affected by the action:

```language=text
[Variables]
  [group1]
    order = FIRST
    family = LAGRANGE
  []
  [group2]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  #---------------------------------------------------------------------
  # Group 1 Neutronics
  #---------------------------------------------------------------------
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
    extra_vector_tags = 'eigen'
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
    extra_vector_tags = 'eigen'
  []
  [inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  []
[]
```

And then the same variables and kernels created with the +NtAction+: 

```language=text
[Nt]
  var_name_base = group
  vacuum_boundaries = 'fuel_bottom mod_bottom right fuel_top mod_top'
  pre_blocks = '0'
  create_temperature_var = false
  eigen = true
[]
```

!! Describe and include an example of how to use the NtAction action.

!syntax description /Nt/NtAction

!syntax parameters /Nt/NtAction
