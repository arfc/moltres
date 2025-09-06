# SNTimeDerivative

!syntax description /Kernels/SNTimeDerivative

## Overview

This object adds the time derivative term of the $S_N$ equation.

!equation
\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d}+\tau_g\hat{\Omega}_d\cdot\Psi^*_{g,d},
  \frac{\Psi_{g,d}}{v_g}\right)_\mathcal{D}

!!## Example Input File Syntax

!syntax parameters /Kernels/SNTimeDerivative

!syntax inputs /Kernels/SNTimeDerivative

!syntax children /Kernels/SNTimeDerivative
