# SNVacuumBC

!syntax description /BCs/SNVacuumBC

## Overview

This object applies the vacuum boundary condition for the $S_N$ neutron transport method.

!equation
\langle\bm{\Psi}^*,\bm{\Psi}\rangle^+ - \langle\bm{\Psi}^*,\bm{\Psi}^\text{inc}\rangle^- =
  \begin{cases}
    \sum^G_{g=1}\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d},
    \hat{\Omega}_d\cdot\hat{n}_b\Psi_{g,d}\right)_{\partial\mathcal{D}},
    & \hat{\Omega}\cdot\hat{n}_b>0,\vec{r}\in\partial\mathcal{D} \\
    0,
    & \hat{\Omega}\cdot\hat{n}_b<0,\vec{r}\in\partial\mathcal{D}
  \end{cases},

where

!equation
\hat{n}_b = \text{ boundary unit vector.}

## Example Input File Syntax

!! Describe and include an example of how to use the SNVacuumBC object.

!syntax parameters /BCs/SNVacuumBC

!syntax inputs /BCs/SNVacuumBC

!syntax children /BCs/SNVacuumBC
