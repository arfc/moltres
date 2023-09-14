# CoupledScalarAdvection

!syntax description /Kernels/CoupledScalarAdvection

## Overview

This object adds the $\vec{u}\cdot\nabla C_i$ advection term of the delayed neutron precursor
equation. $\vec{u}$ can be either constant velocity values defined using the `u_def`, `v_def`, and
`w_def` input parameters or coupled velocity variables using the `u`, `v`, and `w` input
parameters. Note that we applied the product rule and integration by parts to obtain the following
weak form:

!equation
-\int\nabla\psi \cdot \vec{u} C_i\ dV

This form is only valid if the divergence of the velocity is zero, $\nabla \cdot \vec{u} = 0$.
Boundary conditions at the outlet must be defined using [OutflowBC](/OutflowBC.md),
[CoupledOutflowBC](/CoupledOutflowBC.md), or
[CoupledScalarAdvectionNoBCBC](/CoupledScalarAdvectionNoBCBC.md).

## Example Input File Syntax

!! Describe and include an example of how to use the CoupledScalarAdvection object.

!syntax parameters /Kernels/CoupledScalarAdvection

!syntax inputs /Kernels/CoupledScalarAdvection

!syntax children /Kernels/CoupledScalarAdvection
