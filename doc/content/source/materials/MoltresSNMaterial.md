# MoltresSNMaterial

!syntax description /Materials/MoltresSNMaterial

## Overview

This material class preloads and interpolates JSON-based group constant data provided at various
temperature. `MoltresSNMaterial` declares the interpolated group constant data as material
properties which may be accessed by the neutron diffusion kernels to model a reactor and simulate
temperature reactivity feedback.

!table id=group_const caption=Relevant group constants for neutron energy group $g$ or delayed neutron precursor group $i$
| Group constant | Formula | Label |
| - | - | - |
| Macroscopic total cross section | $\Sigma_{t,g}$ | REMXS |
| Macroscopic fission cross section | $\Sigma_{f,g}$ | FISSXS |
| Macroscopic scattering matrix | $\Sigma_{s,l}^{g'\rightarrow g}$ | SPN |
| Inverse neutron velocity | $\frac{1}{v_g}$ | RECIPVEL |
| Fission neutron spectrum (total) | $\chi_{t,g}$ | CHI_T |
| Fission neutron spectrum (prompt) | $\chi_{p,g}$ | CHI_P |
| Fission neutron spectrum (delayed) | $\chi_{d,g}$ | CHI_D |
| Effective delayed neutron fraction | $\beta_{eff}$ | BETA_EFF |
| Delayed neutron precursor decay constant | $\lambda_i$ | DECAY_CONSTANT |

## Example Input File Syntax

!! Describe and include an example of how to use the MoltresSNMaterial object.

!syntax parameters /Materials/MoltresSNMaterial

!syntax inputs /Materials/MoltresSNMaterial

!syntax children /Materials/MoltresSNMaterial
