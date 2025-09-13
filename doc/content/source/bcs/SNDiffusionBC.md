# SNDiffusionBC

!syntax description /BCs/SNDiffusionBC

## Overview

This object applies the boundary source condition for the $S_N$ neutron transport method using the
$P_1$ approximation from scalar flux distributions from the neutron diffusion method.

!equation
\langle\bm{\Psi}^*,\bm{\Psi}\rangle^+ - \langle\bm{\Psi}^*,\bm{\Psi}^\text{inc}\rangle^- =
  \begin{cases}
    \sum^G_{g=1}\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d},
    \hat{\Omega}_d\cdot\hat{n}_b\Psi_{g,d}\right)_{\partial\mathcal{D}},
    & \hat{\Omega}\cdot\hat{n}_b>0,\vec{r}\in\partial\mathcal{D} \\
    \sum^G_{g=1}\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d},
    \hat{\Omega}_d\cdot\hat{n}_b\Psi^\text{inc}_{g,d}\right)_{\partial\mathcal{D}},
    & \hat{\Omega}\cdot\hat{n}_b<0,\vec{r}\in\partial\mathcal{D}
  \end{cases}

where

!equation
\hat{n}_b = \text{ boundary unit vector,} \\
\Psi^\text{inc}_{g,d} = \frac{1}{\sum_{d=1}^{N_d}w_d}
  \left(\phi^\text{diff}_g-3\hat{\Omega}_d\cdot D_g\nabla\phi^\text{diff}_g\right).

## Example Input File Syntax

!! Describe and include an example of how to use the SNDiffusionBC object.

!syntax parameters /BCs/SNDiffusionBC

!syntax inputs /BCs/SNDiffusionBC

!syntax children /BCs/SNDiffusionBC
