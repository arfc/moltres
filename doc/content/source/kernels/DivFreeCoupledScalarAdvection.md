# DivFreeCoupledScalarAdvection

!syntax description /Kernels/DivFreeCoupledScalarAdvection

## Overview

This object adds the $\vec{u}\cdot\nabla C_i$ advection term of the delayed neutron precursor
equation. $\vec{u}$ can be either constant velocity values defined using the `u_def`, `v_def`, and
`w_def` input parameters or coupled velocity variables using the `u`, `v`, and `w` input
parameters.

## Example Input File Syntax

!! Describe and include an example of how to use the DivFreeCoupledScalarAdvection object.

!syntax parameters /Kernels/DivFreeCoupledScalarAdvection

!syntax inputs /Kernels/DivFreeCoupledScalarAdvection

!syntax children /Kernels/DivFreeCoupledScalarAdvection
