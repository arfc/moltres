# SATauMaterial

!syntax description /Materials/SATauMaterial

## Overview

This material class computes strong residuals for the
[Spalart-Allmaras turbulence model equation](theory.md)
and adjusts the $\tau$ stabilization parameter for use in pressure-stabilized (PSPG) and
streamline-upwind (SUPG) kernels. `SATauMaterial` includes all functionality from
[INSADTauMaterial](https://mooseframework.inl.gov/source/materials/INSADTauMaterial.html) for
incompressible Navier-Stokes flow modeling with INSAD kernels from the MOOSE `Navier-Stokes`
physics module.

## Example Input File Syntax

!! Describe and include an example of how to use the SATauMaterial object.

!syntax parameters /Materials/SATauMaterial

!syntax inputs /Materials/SATauMaterial

!syntax children /Materials/SATauMaterial
