# Install Moltres

## 1. Install the Conda MOOSE Environment

Moltres relies on the MOOSE framework. We suggest that users install the MOOSE environment using
Conda packages by following the instructions in the "Install Mambaforge3" and "Install MOOSE Conda
Packages" section of the MOOSE
[installation guide](https://mooseframework.inl.gov/getting_started/installation/conda.html). The
`moose-tools` and `moose-libmesh` Conda package versions which are compatible with the current
MOOSE Git submodule on this repository are `moose-tools=2022.04.18` and `moose-libmesh=2022.06.06`.
We cannot guarantee compatibility with the older MOOSE builds and their corresponding Conda
environments. The latest MOOSE Conda environment is usually compatible unless the MOOSE team
introduces significant changes to the dependencies. We encourage you to post a GitHub issue if you
encounter compatibility issues so that we can rectify it as soon as possible. Run the following
command to install the specified package versions:

```bash
mamba install moose-tools=2022.04.18 moose-libmesh=2022.06.06
```

## 2. Clone Moltres

The Moltres GitHub repository already contains MOOSE and Squirrel as Git submodules, therefore
+there is no need to clone and compile MOOSE separately+. 

Abiding by MOOSE's instructions for setting up a `projects` directory for MOOSE-related content,
users can clone Moltres and download MOOSE as a submodule by running the commands:

```bash
mkdir ~/projects
cd ~/projects
git clone https://github.com/arfc/moltres.git
cd moltres
git checkout master
git submodule init
git submodule update
```

## 3. Compile and Test Moltres id=compile

Before proceeding with this step, check that the Conda MOOSE environment is activated in your
current terminal. This is achieved by running the following command every time you open a new
terminal:

```bash
conda activate moose
```

Finally, compile Moltres and run tests to check your installation using the following commands:

```bash
make -j8
./run_tests -j8
```

!alert note
You may also compile a debug version of Moltres by running `METHOD=dbg make
-j8`. Note that you should replace `8` with the number of processors available
on your machine.

If you encounter any problems with installation, please post on our
[GitHub Discussions](https://github.com/arfc/moltres/discussions) page along with the full error
message so that we can help you troubleshoot your installation.

## 4. Running Moltres Simulations

If you have successfully completed the previous step, you should have a `moltres-opt` executable in
the `~\projects\moltres` directory. To run a Moltres input file, use the following command:

```bash
~/projects/moltres/moltres-opt -i input-file.i
```

To run an input file on more than one processor, use the `mpirun` command as follows:

```bash
mpirun -n 8 ~/projects/moltres/moltres-opt -i input-file.i
```

As above, replace `8` with the number of processors available on your machine.
