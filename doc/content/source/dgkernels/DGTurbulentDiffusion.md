# DGTurbulentDiffusion

!syntax description /DGKernels/DGTurbulentDiffusion

## Overview

This object adds the $-\nabla D_t \nabla u$ turbulent diffusion term based on turbulence predicted
by the Spalart-Allmaras turbulence model. The $\epsilon$ and $\sigma$
values may be adjusted to attain the desired level of artificial diffusion for eliminating
undershoots and overshoots in the solution. 

## Example Input File Syntax

!! Describe and include an example of how to use the DGTurbulentDiffusion object.

!syntax parameters /DGKernels/DGTurbulentDiffusion

!syntax inputs /DGKernels/DGTurbulentDiffusion

!syntax children /DGKernels/DGTurbulentDiffusion
