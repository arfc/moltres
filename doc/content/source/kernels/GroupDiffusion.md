# GroupDiffusion

!syntax description /Kernels/GroupDiffusion

## Overview

This object adds the $- \nabla \cdot D_g \nabla \phi_g$ diffusion term of the multigroup neutron
diffusion equations. Note that we have applied integration by parts to obtain the following weak
form:

!equation
\int \nabla \psi \cdot D_g \nabla \phi_g\ dV

!!## Example Input File Syntax

!syntax parameters /Kernels/GroupDiffusion

!syntax inputs /Kernels/GroupDiffusion

!syntax children /Kernels/GroupDiffusion
