# SNFission

!syntax description /Kernels/SNFission

## Overview

This object adds the fission source term of the $S_N$ equation.

!equation
\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d}+\tau_g\hat{\Omega}_d\cdot\nabla\Psi^*_{g,d},
  \frac{1}{w}\bar{\chi}_g\sum^G_{g'=1}\nu\Sigma_{f,g'}\phi_{g'}\right)_\mathcal{D}

!!## Example Input File Syntax

!syntax parameters /Kernels/SNFission

!syntax inputs /Kernels/SNFission

!syntax children /Kernels/SNFission
