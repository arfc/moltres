# SNExternalSource

!syntax description /Kernels/SNExternalSource

## Overview

This object adds the external neutron source term of the $S_N$ equation.

!equation
\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d}+\tau_g\hat{\Omega}_d\cdot\nabla\Psi^*_{g,d},
  Q^{\text{ext}}\right)_\mathcal{D}

!!## Example Input File Syntax

!syntax parameters /Kernels/SNExternalSource

!syntax inputs /Kernels/SNExternalSource

!syntax children /Kernels/SNExternalSource
