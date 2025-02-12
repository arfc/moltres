[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.801822.svg)](https://doi.org/10.5281/zenodo.801822)
[![DOI](http://joss.theoj.org/papers/10.21105/joss.00298/status.svg)](https://doi.org/10.21105/joss.00298)

Moltres
=====

Moltres is an open-source MOOSE-application code designed for simulation of molten salt
reactors and other advanced reactors.

## Documentation

Moltres documentation can be found at [https://arfc.github.io/moltres/](https://arfc.github.io/moltres/) and 
[https://arfc.github.io/software/moltres](https://arfc.github.io/software/moltres).
Outlines of the kernels and boundary
conditions used to construct the Moltres governing equations can be found on the
[Moltres wiki](https://arfc.github.io/software/moltres/wiki/). Breakdown of a
full-fledged Moltres input file can be found
[here](https://arfc.github.io/software/moltres/wiki/input_example/). New Moltres
users who have never used MOOSE before are encouraged to check-out the MOOSE
[website](https://mooseframework.inl.gov/) and
workshop [slides](https://mooseframework.inl.gov/workshop/index.html#/) for
tutorials and examples to help understand the
underlying Moltres components.

## Install

Installation instructions can be found at
[https://arfc.github.io/moltres/getting_started/installation.html](https://arfc.github.io/moltres/getting_started/installation.html)

## Development

If you ever want to contribute changes to the Moltres repository, make sure you
run `scripts/install-format-hook.sh` before making any commits. This will
ensure that any commits you make adhere to the MOOSE/Moltres C++ style. Pull
requests should be submitted to the `devel` branch of the `arfc/moltres`
repository. Each pull request is automatically tested for style and for whether
it breaks any of the core Moltres physics capabilities.

A full list of contributing guidelines can be found 
[here](https://github.com/arfc/moltres/blob/devel/CONTRIBUTING.md).

## Contact

Please communicate issues with the development team through GitHub issues and 
pull requests. 
