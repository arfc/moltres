# INSADMomentumTurbulentViscous

!syntax description /Kernels/INSADMomentumTurbulentViscous

## Overview

This object adds the $-\mu_t\left(\nabla\vec{u}+\left(\nabla\vec{u}\right)^T\right)$ turbulent
viscous term of the incompressible Navier-Stokes momentum equation under the INSAD implementation.
The term is integrated by parts. Only the traction form is compatible with the Spalart-Allmaras
turbulence model because $\mu_t$ is generally not constant in space.

## Example Input File Syntax

!! Describe and include an example of how to use the INSADMomentumTurbulentViscous object.

!syntax parameters /Kernels/INSADMomentumTurbulentViscous

!syntax inputs /Kernels/INSADMomentumTurbulentViscous

!syntax children /Kernels/INSADMomentumTurbulentViscous
