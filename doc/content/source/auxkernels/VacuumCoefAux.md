# VacuumCoefAux

!syntax description /AuxKernels/VacuumCoefAux

## Overview

Computes the boundary correction coefficient for nonlinear diffusion acceleration of the $S_N$
method or the hybrid $S_N$-diffusion method.

!equation
\gamma_g =
  \frac{\sum_{\hat{\Omega}_d\cdot\hat{n}_b > 0}w_d |\hat{\Omega}_d\cdot\hat{n}_b |
  \Psi_{g,d}}{\sum^{N_d}_{d=1}w_d\Psi_{g,d}}

## Example Input File Syntax

!! Describe and include an example of how to use the VacuumCoefAux object.

!syntax parameters /AuxKernels/VacuumCoefAux

!syntax inputs /AuxKernels/VacuumCoefAux

!syntax children /AuxKernels/VacuumCoefAux
