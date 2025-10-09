# SNDelayedNeutronSource

!syntax description /Kernels/SNDelayedNeutronSource

## Overview

This object adds the delayed neutron source term of the $S_N$ equation.

!equation
\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d}+\tau_g\hat{\Omega}_d\cdot\nabla\Psi^*_{g,d},
  \frac{1}{w}\sum ^I_{i=1}\chi_{d,g}\lambda_i C_i\right)_\mathcal{D}

!!## Example Input File Syntax

!syntax parameters /Kernels/SNDelayedNeutronSource

!syntax inputs /Kernels/SNDelayedNeutronSource

!syntax children /Kernels/SNDelayedNeutronSource
