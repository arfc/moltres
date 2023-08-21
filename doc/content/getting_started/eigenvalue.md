# Eigenvalue Calculation with NtAction

The input files associated with this tutorial are
[moltres/tutorial/eigenvalue/nts.i](https://github.com/arfc/moltres/blob/devel/tutorial/eigenvalue/nts.i) and
[moltres/tutorial/eigenvalue/nts-action.i](https://github.com/arfc/moltres/blob/devel/tutorial/eigenvalue/nts-action.i).
Refer to [Tutorial 2a](getting_started/input_syntax.md) for an introduction on the input file
syntax and in-depth descriptions of every input parameter.

As mentioned in Tutorial 2a, this example involves a simple 2-D axisymmetric core model of the
Molten Salt Reactor Experiment (MSRE) that was developed at Oak Ridge National Laboratory. The
figure below shows the 2-D MSRE model for this tutorial. It is a 70.8 cm by 169 cm rectangle that
is axisymmetric about the left boundary. The domain consists of 14 fuel channels, alternating with
14 solid graphite moderator regions, represented in the figure by gray and red rectangles,
respectively.

!media media/msre_2d.png
       id=msre-2d
       caption=2-D axisymmetric model of the MSRE
       style=width:35%;text-align:center;margin-left:auto;margin-right:auto;

For multiplication factor ($k$) eigenvalue calculations, Moltres relies on the
`InversePowerMethod` or `NonlinearEigen` executioners from MOOSE. These executioners solve the
neutron diffusion equations set up as an eigenvalue problem to find $k$ and the neutron flux
distribution, which occur as the absolute minimum eigenvalue $1/k$ and the corresponding
eigenvector of the system. The mathematical background and documentation for these executioners can
be found [here](https://mooseframework.inl.gov/source/executioners/InversePowerMethod.html). This
tutorial uses the `InversePowerMethod` executioner which applies the inverse power method.


