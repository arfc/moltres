# Theory and Methodology

The *two-step procedure* is a common approach for multiphysics full-core nuclear reactor analysis.
These steps are:

+Step 1:+ Generate neutron group constant data with a lattice or full-core reactor model on a
high-fidelity, continuous/fine-energy neutronics software at various reactor states.

+Step 2:+ Use an intermediate-fidelity, computationally cheaper neutronics software with the
neutron group constant data to perform multiphysics reactor analysis.

Moltres falls under Step 2 of the two-step procedure. Moltres solves the conventional multigroup
neutron diffusion and delayed neutron precursor equations. The precursor equations include
precursor drift terms to account for precursor movement under molten salt flow. Moltres is built on
the MOOSE finite element framework, enabling highly flexible and scalable reactor simulations.

## Multigroup Neutron Diffusion

The neutron diffusion equation is an approximation to the Boltzmann transport equation, and is derived by taking the zeroth and first moment with respect to $\hat\Omega$, the direction of neutron travel. 
A full derivation of the diffusion equation will not be presented here for conciceness. Importantly, when taking the first moment of the transport equation the angular flux is assumed to be linearly anisotropic. 
The approximations reduce the phase space through the elimination of angular dependence, but the resulting equation also has reduced fidelity when compared to the transport equation, particularly in regions where the neutron flux has strong angular dependence. 
A non-exhaustive list of regions in which the neutron flux strongly depends on angle is near material interfaces between materials with highly dissimilar neutronic properties, within strong absorbers, and within near-void regions.
As a dertiministic method, the neutron diffusion method also requires discretization of the continuous energy dependence into energy groups consisting of non-overlapping, finite energy ranges across the entire energy spectrum. 
This energy discretization creates a system of equations referred to as the multigroup neutron diffusion equations: 

!equation
\frac{1}{v_g} \frac{\partial\phi_g}{\partial t}-\nabla \cdot D_g \nabla \phi_g +\Sigma^R_{g} \phi_g =\sum^G_{g \neq g'} {\Sigma^s_{g'\rightarrow g} \phi_{g'}}+ \chi^p_{g} \sum^G_{g'=1} {\left(1- \beta \right) \nu_{g'} \Sigma^f_{g'}\phi_{g'} }+\chi^d_{g} \sum^I_i {\lambda_i C_i}

where the precursor distributions are governed by:

!equation
\frac{\partial C_i}{\partial t}=\sum^G_{g'=1}{\beta_i \nu \Sigma^f_{g'}\phi_{g'}}-\lambda_i C_i-\vec{u} \cdot \nabla C_i


Notably, there are two production terms of neutrons, the prompt fission source and delayed neutron precursor decay source. 
The first term describes the neutrons immediately born from fission, and the second term describes the neutrons born from the radioactive decay of neutron-emitting radionuclides, commonly called delayed neutron precursors. 
The multigroup neutron diffusion equations are generally impossible to solve analytically for realistic problems and are therefore typically solved with numerical methods. 
Moltres utilizes the [Finite Element Method](https://mooseframework.inl.gov/help/faq/what_is_fem.html) (FEM) capabilities provided by the MOOSE framework.  
In FEM, the spatial domain is discretized into finite mesh elements and the time dependence is modeled through discrete time steps and time integration methods. 

!table id=diff_table caption= Terms in multi-group neutron diffusion equation and their associated kernels
|Term in Multi-Group Diffusion Equation | Associated Kernel |Definition of Term|
| - | - | - |
| [!eq](\frac{1}{v_g} \frac{\partial\phi_g}{\partial t}) | [NtTimeDerivative](NtTimeDerivative.md) | Time rate of change of energy group g |
| [!eq](- \nabla \cdot D_g \nabla \phi_g) | [GroupDiffusion](GroupDiffusion.md) | Streaming term of energy group g |
| [!eq](\Sigma^R_{g} \phi_g) | [SigmaR](SigmaR.md) | Removal from energy group g |
| [!eq](\sum^G_{g \neq g'} {\Sigma^s_{g'\rightarrow g} \phi_{g'}}) | [InScatter](InScatter.md) | In-scattering into energy group g | 
| [!eq](\chi^p_{g} \sum^G_{g'=1} {\left[1- \beta \right] \nu_{g'} \Sigma^f_{g'}\phi_{g'} } ) | [CoupledFissionKernel](CoupledFissionKernel.md) | Prompt fission neutron source |
| [!eq](\chi^d_{g} \sum^I_i {\lambda_i C_i}) | [DelayedNeutronSource](DelayedNeutronSource.md) | Delayed fission neutron source |

!table id=prec_table caption= Terms in delayed neutron precursor equation and their associated kernels
| Term in Delayed Precursor Equation | Associated Kernel(s) | Definition of Term |
| - | - | - |
| [!eq](\frac{\partial C_i}{\partial t}) | [ScalarTransportTimeDerivative](ScalarTransportTimeDerivative.md) | Time rate of change of precursor population | 
| [!eq](\sum^G_{g'=1}{\beta_i \nu \Sigma^f_{g'}\phi_{g'}}) | [PrecursorSource](PrecursorSource.md) | Production of precursor from fission 
| [!eq](-\lambda_i C_i) | [PrecursorDecay](PrecursorDecay.md) | Loss of precusors due to radioactive decay | 
| [!eq](-\vec{u} \cdot \nabla C_i) | [DGCoupledAdvection](DGCoupledAdvection.md), [DGFunctionConvection](DGFunctionConvection.md), [DGConvection](https://mooseframework.inl.gov/source/dgkernels/DGConvection.html) | Advection of the precursors |

For the advective term in the delayed neutron precursor equation, $-\vec{u}\cdot \nabla C_i$, there are three kernels. DGCoupledAdvection is used when the velocity is a variable in the simulation, DGFunctionConvection is used when the velocity is a function, and DGConvection is for when the velocity is constant.

## Heat Transfer and Fluid Flow

Moltres compiles with the MOOSE
[Navier-Stokes](https://mooseframework.inl.gov/modules/navier_stokes/index.html) and
[Heat Transfer](https://mooseframework.inl.gov/modules/heat_transfer/index.html) modules for flow
modeling capabilities by default. Moltres couples with these modules natively because they are all
built on the MOOSE framework.

Past work with Moltres have modeled incompressible salt flow and temperature advection-diffusion
with the
[INS or INSAD implementations](https://mooseframework.inl.gov/modules/navier_stokes/cgfe.html) of
the Navier-Stokes equations [!citep](peterson_overview_2018). Coupling with the compressible or
finite volume implementations within the Navier-Stokes module are likely possible, but have not
been demonstrated yet.

For turbulence modeling, Moltres has a Spalart-Allmaras (SA) turbulence model
[!citep](spalart_one-equation_1992) implementation with streamline-upwind Petrov-Galerkin (SUPG)
stabilization. On balance the SA model is a complete (does not require prior knowledge of the
actual turbulence behavior) and computationally efficient turbulence model for approximating
wall-bounded turbulent flows. The SA model implementation in Moltres is
compatible with the INSAD implementation only. Additionally, Moltres contains turbulent diffusion
physics kernels for temperature and the delayed neutron precursors.

The SA model in Moltres follows the (SA-noft2-R) implementation as described on the NASA
[Turbulence Modeling Resource](https://turbmodels.larc.nasa.gov/spalart.html) website. The $f_{t2}$
term is togglable using the `use_ft2_term` input parameter (false by default). Moltres'
implementation solves for the modified dynamic viscosity $\tilde{\mu}$ which can be easily obtained
using $\nu=mu/\rho$ to convert $\nu$ to $\mu$ as follows:

!equation id=sa-equation
\rho \frac{\partial\tilde{\mu}}{\partial t} + \rho \mathbf{u}\cdot\nabla\tilde{\mu} = \rho c_{b1}
\left(1-f_{t2}\right)\tilde{S}\tilde{\mu} + \frac{1}{\sigma}\{\nabla\cdot\left[\left(\mu+
\tilde{\mu}\right)\nabla\tilde{\mu}\right] + c_{b2}|\nabla\tilde{\mu}|^2\} - \left(c_{w1}f_w -
\frac{c_{b1}}{\kappa^2}f_{t2}\right)\left(
\frac{\tilde{\mu}}{d}\right)^2

where

!equation
\mu_t = \tilde{\mu}f_{v1} = \text{turbulent eddy viscosity}, \hspace{4cm}
f_{v1} = \frac{\chi^3}{\chi^3 + c_{v1}^3}, \hspace{4cm}
\chi = \frac{\tilde{\mu}}{\mu}, \hspace{2cm} \\ \hspace{1cm}
\tilde{S} = \Omega + \frac{\tilde{\nu}}{\kappa^2 d^2} f_{v2}, \hspace{6cm}
f_{v2} = 1 - \frac{\chi}{1+\chi f_{v1}}, \hspace{2cm}
\Omega = \sqrt{2W_{ij}W_{ij}} = \text{vorticity magnitude}, \\
W_{ij} = \frac{1}{2}\left(\frac{\partial u_i}{\partial x_j} - \frac{\partial u_j}{\partial x_i}
\right), \hspace{5cm}
f_w = g\left(\frac{1 + c_{w3}^6}{g^6 + c_{w3}^6}\right)^{1/6}, \hspace{3cm}
g = r + c_{w2}\left(r^6 - r\right), \\
r = \text{min}\left(\frac{\tilde{\nu}}{\tilde{S}\kappa^2d^2}, 10\right), \hspace{5cm}
f_{t2} = c_{t3} \exp{\left(-c_{t4}\chi^2\right)}, \hspace{6cm}

and the constants are:

!equation
\sigma = \frac{2}{3}, \ c_{b1} = 0.1355, \ c_{b2} = 0.622, \ \kappa = 0.41, \
c_{w1} = \frac{c_{b1}}{\kappa^2} + \frac{1+c_{b2}}{\sigma}, \ c_{w2} = 0.3, \ c_{w3} = 2, \
c_{v1} = 7.1, \ c_{t3} = 1.2, \ c_{t4} = 0.5 \ \text{.}

We performed simple verification and validation tests of the SA model in Moltres using reference
problems for channel, pipe, and backward-facing step flow. The Moltres test dataset is available on
[Zenodo](https://doi.org/10.5281/zenodo.10059649). The following subsections show plots comparing
results from Moltres to reference data from literature.

#### Turbulent Channel Flow Verification Test

The channel flow test is based on the direct numerical simulation (DNS) of turbulent channel flow
with $Re_\tau\approx395$
by [!cite](moser_direct_1999). The Moltres input file for this problem may be found in
`moltres/problems/2023-basic-turbulence-cases/channel-flow/`.

!media channel_vel.png
       id=channel-vel
       caption=Normalized velocity distribution across the channel.
       style=width:33%;padding:10px;float:left;

!media channel_nondim.png
       id=channel-nondim
       caption=Dimensionless velocity vs dimensionless wall distance.
       style=width:33%;padding:10px;float:left;

!media channel_stress.png
       id=channel-stress
       caption=Normalized stress distribution across the channel.
       style=width:33%;padding:10px;float:left;

#### Turbulent Pipe Flow Validation Test

The pipe flow test is based on the turbulent pipe flow experiment with $Re\approx 40,000$ by
[!cite](laufer_structure_1954). The Moltres input file for this problem may be found in
`moltres/problems/2023-basic-turbulence-cases/pipe-flow/`.

!media pipe_vel.png
       id=pipe-vel
       caption=Normalized velocity distribution across the pipe.
       style=width:33%;padding:10px;float:left;

!media pipe_nondim.png
       id=pipe-nondim
       caption=Dimensionless velocity vs dimensionless wall distance.
       style=width:33%;padding:10px;float:left;

!media pipe_stress.png
       id=pipe-stress
       caption=Normalized turbulent shear stress distribution across the channel.
       style=width:33%;padding:10px;float:left;

#### Backward-Facing Step Flow Validation Test

The backward-facing step (BFS) flow test is based on the BFS flow experiment with $Re\approx
36,000$ by
[!cite](driver_features_1985). The Moltres input file for this problem may be found in
`moltres/problems/2023-basic-turbulence-cases/pipe-flow/`.
The SA model implementation in Moltres performs largely similarly to the reference SA model results
provided on the [Turbulence Modeling Resource](https://turbmodels.larc.nasa.gov/spalart.html)
website.

!table caption=Flow reattachment length estimates (normalized by step height $H$) id=re-table
| Reference experimental data | Reference SA model | Moltres |
| :- | :- | :- |
| $6.26 \pm 0.10$ | $6.1$ | $6.36$ |

!media bfs_upstream_vel.png
       id=bfs-upstream-vel
       caption=Normalized velocity distribution at $x/H$ equal -4 (upstream of step).
       style=width:33%;padding:10px;float:left;

!media bfs_downstream_vel.png
       id=bfs-downstream-vel
       caption=Normalized velocity distribution at various $x/H$ locations (downstream of step).
       style=width:33%;padding:10px;float:left;

!media bfs_cf.png
       id=bfs-cf
       caption=Skin friction coefficient along the bottom wall.
       style=width:33%;padding:10px;float:left;

!media bfs_cp.png
       id=bfs-cp
       caption=Skin pressure coefficient along the bottom wall.
       style=width:33%;padding:10px;margin-left:17%;float:left;

!media bfs_stress.png
       id=bfs-stress
       caption=Normalized turbulent shear stress distribution at various $x/H$ locations
       (downstream of step).
       style=width:33%;padding:10px;margin-right:17%;float:left;

!media bfs.png
       id=bfs
       caption=Close-up of the velocity magnitude and streamlines around the backward-facing step
       style=width:80%;float:left;margin-left:10%;margin-right:10%;

!bibtex bibliography
