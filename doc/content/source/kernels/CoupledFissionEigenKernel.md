# CoupledFissionEigenKernel

!syntax description /Kernels/CoupledFissionEigenKernel

## Overview

This object adds the $\frac{\chi_g^p}{k} \sum_{g' = 1}^G (1 - \beta) \nu \Sigma_{g'}^f \phi_{g'}$
neutron source term of the multigroup neutron diffusion equations for the $k$-eigenvalue problem.
If `account_delayed=false` (i.e.
no delayed neutron precursor modeling), the $\frac{\chi_g^t}{k} \sum_{g' = 1}^G \nu \Sigma_{g'}^f
\phi_{g'}$ form is used instead.

## Example Input File Syntax

!! Describe and include an example of how to use the CoupledFissionEigenKernel object.

!syntax parameters /Kernels/CoupledFissionEigenKernel

!syntax inputs /Kernels/CoupledFissionEigenKernel

!syntax children /Kernels/CoupledFissionEigenKernel
