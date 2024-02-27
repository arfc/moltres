# SATauStabilized3Eqn

!syntax description /Materials/SATauStabilized3Eqn

## Overview

This material class computes strong residuals for the temperature advection-diffusion equation
and adjusts the $\tau$ stabilization parameter for use in pressure-stabilized (PSPG) and
streamline-upwind (SUPG) kernels. `SATauStabilized3Eqn` includes all functionality from
[INSADStabilized3Eqn](https://mooseframework.inl.gov/source/materials/INSADStabilized3Eqn.html) for
thermal-hydraulics modeling with INSAD kernels from the MOOSE `Navier-Stokes`
physics module and [SATauMaterial](SATauMaterial.md) for Spalart-Allmaras turbulence modeling.

## Example Input File Syntax

!! Describe and include an example of how to use the SATauStabilized3Eqn object.

!syntax parameters /Materials/SATauStabilized3Eqn

!syntax inputs /Materials/SATauStabilized3Eqn

!syntax children /Materials/SATauStabilized3Eqn
