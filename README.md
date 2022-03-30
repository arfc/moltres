[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.801822.svg)](https://doi.org/10.5281/zenodo.801822)
[![DOI](http://joss.theoj.org/papers/10.21105/joss.00298/status.svg)](https://doi.org/10.21105/joss.00298)

Moltres
=====

Moltres is a MOOSE-application code designed for simulation of molten salt
reactors.

## Use

Moltres documentation can be found
[here](http://arfc.github.io/software/moltres). Doxygen pages are
[here](https://arfc.github.io/moltres/classes.html). Outlines of the kernels and boundary
conditions used to construct the Moltres governing equations can be found on the
[Moltres wiki](http://arfc.github.io/software/moltres/wiki/). Breakdown of a
full-fledged Moltres input file can be found
[here](http://arfc.github.io/software/moltres/wiki/input_example/). New Moltres
users who have never used MOOSE before are encouraged to check-out the MOOSE
[website](https://mooseframework.inl.gov/) and
workshop [slides](https://mooseframework.inl.gov/workshop/index.html#/) for
tutorials and examples to help understand the
underlying Moltres components.

## Install

Moltres relies on the MOOSE framework. We suggest that users install the MOOSE 
environment using Conda Packages by following the instructions in the `Install 
MOOSE Conda Packages` section of
https://mooseframework.inl.gov/getting_started/installation/conda.html.
The `moose-tools` and `moose-libmesh` Conda package versions which are
compatible with the current MOOSE Git submodule on this repository are
`moose-tools=2021.07.14` and `moose-libmesh=2021.07.14`. We cannot guarantee
compatibility with the older/newer MOOSE builds and their corresponding Conda
environments. Run the following command to install the specified package
versions:

```bash
mamba install moose-tools=2021.07.14 moose-libmesh=2021.07.14
```

The Moltres repository already contains MOOSE and Squirrel as Git
submodules, therefore there is **no need to separately clone and compile
MOOSE.** Instead, users can install Moltres and the relevant MOOSE
components by running the commands below in a shell after changing into the
directory which will hold the Moltres directory (perhaps `~/projects`).

```bash
git clone https://github.com/arfc/moltres
cd moltres
git submodule init
git submodule update
make -j8
```

You may also compile a debug version of Moltres by replacing the last line in 
the code block above with `METHOD=dbg make
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

A full list of contributing guidelines can be found 
[here](https://github.com/arfc/moltres/blob/devel/CONTRIBUTING.md).

## Contact

Please post to our [discussion list](https://groups.google.com/g/moltres-users)
at moltres-users@googlegroups.com.
