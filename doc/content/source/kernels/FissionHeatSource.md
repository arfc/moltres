# FissionHeatSource

!syntax description /Kernels/FissionHeatSource

## Overview

This object adds the $\frac{P}{\int_V \sum_{g' = 1}^G \epsilon_{g'} \Sigma_{g'}^f \phi_{g'} dV} \sum_{g' = 1}^G \epsilon_{g'} \Sigma_{g'}^f \phi_{g'}$ heat source term of the steady-state temperature
advection-diffusion equation with coupled neutron fluxes from a $k$-eigenvalue calculation. The
`power` input parameter allows the user to set a fixed thermal power output regardless of the
neutron flux normalization factor. Alternatively, one may choose to normalize the neutron flux
instead using the `normalization` and `normal_factor` parameters for the
[InversePowerMethod](https://mooseframework.inl.gov/source/executioners/InversePowerMethod.html) or
[NonlinearEigen](https://mooseframework.inl.gov/source/executioners/NonlinearEigen.html)
executioners.

## Example Input File Syntax

!! Describe and include an example of how to use the FissionHeatSource object.

!syntax parameters /Kernels/FissionHeatSource

!syntax inputs /Kernels/FissionHeatSource

!syntax children /Kernels/FissionHeatSource
