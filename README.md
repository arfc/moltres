Moltres
=====

Moltres is a MOOSE-application code design for simulation of molten salt
reactors. 

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
-j8`.

## Development

If you ever want to contribute changes to the Moltres repository, make sure you
run `./scripts/install-format-hook.sh` before making any commits. This will
ensure that any commits you make adhere to the MOOSE/Moltres C++ style.

## Contact 

Contact Alex Lindsay at al007@illinois.edu for more information or
even better, please post to our discussion list at
moltres-users@googlegroups.com.
