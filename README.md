[![DOI](https://zenodo.org/badge/65950715.svg)](https://zenodo.org/badge/latestdoi/65950715)

Moltres
=====

Moltres is a MOOSE-application code designed for simulation of molten salt
reactors.

## Use

Moltres documentation can be found at arfc/github.io/software/moltres. Doxygen
pages are [here](https://arfc.github.io/moltres/). Outlines of the kernels and
boundary conditions used to construct the Moltres governing equations can be
found on the
[Moltres wiki](http://arfc.github.io/software/moltres/wiki/). Breakdown of a
full-fledged Moltres input file can be found
[here](http://arfc.github.io/software/moltres/wiki/input_example/). New Moltres users who
have never used MOOSE before are encouraged to check-out its [wiki](http://mooseframework.org/wiki/),
[tutorials](http://mooseframework.org/wiki/MooseTutorials/), and
[examples](http://mooseframework.org/wiki/MooseExamples/) to help understand the
underlying Moltres components.

## Install

Moltres relies on the MOOSE framework. To install MOOSE please follow the
instructions at http://mooseframework.org/getting-started/. After installing
MOOSE, you can install Moltres by running the following commands in a shell
after changing into the directory holding your MOOSE directory (perhaps `~/projects`):

```bash
git clone https://github.com/arfc/moltres
cd moltres
git submodule init
git submodule update
make -j8
```

You may also compile a debug version of Moltres by running `METHOD=dbg make
-j8`. Note that you should replace `8` with the number of processors available
on your machine.

## Testing

To ensure that Moltres is functioning properly, run `./run_tests -j8` from the
root of the Moltres directory.

## Development

If you ever want to contribute changes to the Moltres repository, make sure you
run `scripts/install-format-hook.sh` before making any commits. This will
ensure that any commits you make adhere to the MOOSE/Moltres C++ style. Pull
requests should be submitted to the `devel` branch of the `arfc/moltres`
repository. Each pull request is automatically tested for style and for whether
it breaks any of the core Moltres physics capabilities.

## Contact

Contact Alex Lindsay at al007@illinois.edu for more information or
even better, please post to our discussion list at
moltres-users@googlegroups.com.
