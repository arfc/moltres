# VacuumCorrectionBC

!syntax description /BCs/VacuumCorrectionBC

## Overview

This object applies boundary correction along vacuum boundaries to the neutron diffusion equation
for nonlinear diffusion acceleration of the $S_N$ method or the hybrid $S_N$-diffusion method.

!equation
\left(\phi^*,\gamma_g\phi_g\right)_{\partial\mathcal{D}}

where

!equation
\gamma_g =
  \frac{\sum_{\hat{\Omega}_d\cdot\hat{n}_b > 0}w_d |\hat{\Omega}_d\cdot\hat{n}_b |
  \Psi_{g,d}}{\sum^{N_d}_{d=1}w_d\Psi_{g,d}}

## Example Input File Syntax

!! Describe and include an example of how to use the VacuumCorrectionBC object.

!syntax parameters /BCs/VacuumCorrectionBC

!syntax inputs /BCs/VacuumCorrectionBC

!syntax children /BCs/VacuumCorrectionBC
