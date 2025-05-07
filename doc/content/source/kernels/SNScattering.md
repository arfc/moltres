# SNScattering

!syntax description /Kernels/SNScattering

## Overview

This object adds the scattering source term of the $S_N$ equation.

!equation
\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d}+\tau_g\hat{\Omega}_d\cdot\nabla\Psi^*_{g,d},
  \sum^G_{g'=1}\sum^L_{l=0}\Sigma^{g'\rightarrow g}_{s,l}\sum^l_{m=-l}
  \frac{2l+1}{w}Y_{l,m}(\hat{\Omega}_d)\phi_{g',l,m}\right)_\mathcal{D}

!!## Example Input File Syntax

!syntax parameters /Kernels/SNScattering

!syntax inputs /Kernels/SNScattering

!syntax children /Kernels/SNScattering
