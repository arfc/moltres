# GroupDrift

!syntax description /Kernels/GroupDrift

## Overview

This object adds the neutron drift term to the neutron diffusion equation for nonlinear diffusion
acceleration of the $S_N$ method or the hybrid $S_N$-diffusion method.

!equation
\left(\nabla\phi^*,\vec{D}_g\phi_g\right)

where

!equation
\vec{D}_g = \frac{\sum^{N_d}_{d=1}w_d\left(\tau_g\hat{\Omega}_d\hat{\Omega}_d\cdot\nabla\Psi_{g,d}
  + \left(\tau_g\Sigma_{t,g}-1\right)\hat{\Omega}_d\Psi_{g,d}
  - \tau_g\sum^G_{g'=1}\Sigma^{g'\rightarrow g}_{s,1}\hat{\Omega}_d\Psi_{g',d}
  - D_g\nabla\Psi_{g,d}\right)}{\sum^{N_d}_{d=1}w_d\Psi_{g,d}}

!!## Example Input File Syntax

!syntax parameters /Kernels/GroupDrift

!syntax inputs /Kernels/GroupDrift

!syntax children /Kernels/GroupDrift
