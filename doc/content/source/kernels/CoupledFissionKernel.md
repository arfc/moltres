# CoupledFissionKernel

!syntax description /Kernels/CoupledFissionKernel

## Overview

This object adds the $\chi_g^p \sum_{g' = 1}^G (1 - \beta) \nu \Sigma_{g'}^f \phi_{g'}$
neutron source term of the multigroup neutron diffusion equations. If `account_delayed=false` (i.e.
no delayed neutron precursor modeling), the $\chi_g^t \sum_{g' = 1}^G \nu \Sigma_{g'}^f \phi_{g'}$
form is used instead.

## Example Input File Syntax

!! Describe and include an example of how to use the CoupledFissionKernel object.

!syntax parameters /Kernels/CoupledFissionKernel

!syntax inputs /Kernels/CoupledFissionKernel

!syntax children /Kernels/CoupledFissionKernel
