In this problem, a 2D axisymmetric reactor with salt fraction reminiscent of the MSRE is started with a single control rod
lowered to 20 cm above the origin. The ODE given in ScalarKernels drives the CR to move up to the 100 cm position over time.
Here, it can be seen that the neutronics solution moves on a much faster timescale than the rod movement timescale. This leads
to overstepping the neutronics part, giving negative fluxes.
