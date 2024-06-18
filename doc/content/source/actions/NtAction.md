# NtAction

!alert note
This action sets up ONLY the variables, kernels, and BCs for neutron diffusion,
NOT the variables, kernels, and BCs for precursor tracking. 
To generate these, see [PrecursorAction](PrecursorAction.md).

!syntax description /Nt/NtAction

## Overview

```NtAction``` greatly reduces input file length and complexity by automatically
setting up variables, kernels, and BCs for the neutron diffusion equations.
When including only the required input parameters, this action constructs the
neutron group variables and their associated kernels:
[SigmaR](SigmaR.md), [GroupDiffusion](GroupDiffusion.md),
[InScatter](InScatter.md), and [CoupledFissionKernel](CoupledFissionKernel.md).
With ```account_delayed``` set to ```True```,
[DelayedNeutronSource](DelayedNeutronSource.md) is also constructed,
otherwise this kernel is not constructed and the simulation does not consider
the delayed neutron precursor effects on the neutron flux distributions.

For more information regarding the use of ```NtAction``` please refer to the
tutorials located [here](tutorials.md), specifically the +Multiphysics Reactor
Simulations+ section.

## Example Input File Syntax

An example input file without the ```NtAction```, showing only the portion
affected by the action:

!listing tutorial/eigenvalue/nts.i 
	start=Variables end=Materials remove=Precursors


And then the same information created with the ```NtAction```: 

!listing tutorial/eigenvalue/nts-action.i
	block=Nt

!syntax description /Nt/NtAction

!syntax parameters /Nt/NtAction
