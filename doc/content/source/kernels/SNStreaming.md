# SNStreaming

!syntax description /Kernels/SNStreaming

## Overview

This object adds the streaming term of the $S_N$ equation.

!equation
\sum^{N_d}_{d=1}w_d\left(\hat{\Omega}_d\cdot\nabla\Psi^*_{g,d},\tau_g\hat{\Omega}
  \cdot\nabla\Psi_{g,d}-(1-\tau_g\Sigma_{t,g})\Psi_{g,d}\right)_\mathcal{D}

!!## Example Input File Syntax

!syntax parameters /Kernels/SNStreaming

!syntax inputs /Kernels/SNStreaming

!syntax children /Kernels/SNStreaming
