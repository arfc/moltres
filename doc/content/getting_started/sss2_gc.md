# Group Constant File Generation With `extractSerpent2GCs.py`

## Example problem: Two-region MSRE model

Here, group constants are taken from an example Serpent output using PyNE. These will be used to
generate text-based group constant files using `extractSerpent2GCs.py`. The relevant files for this
tutorial may be found in the
[`MSRE_group_constants`](https://github.com/arfc/moltres/tree/devel/tutorial) tutorial directory.

Moltres is *not* a spatially homogenized diffusion code. MOOSE is made for running +big+, +intense+
problems, using modern HPC. Because there is no desire to homogenize spatially here, the materials
that the user would like used in Moltres should each fill their own infinite universe. Then, this
universe should have its group constants generated using the `set gcu <material universe numbers>`
option in Serpent 2.

It's important that universe 0, the main universe, is not included. Serpent takes tallies for group
constants in the first universe it identifies, so including 0 means that no further universes will
be included in GC generation. (please double check this on the
[serpent forum](https://ttuki.vtt.fi/serpent)).

### Step 1: Generating Group Constants with Serpent 2

In this directory, we provide a Serpent input file for simulating ORNL's Molten Salt Reactor
Experiment, documented largely on websites like [this](http://www.energyfromthorium.com/pdf/).

For group constant generation, Cole Gentry found that a 4 group structure with three thermal groups
and one fast group works well in the graphite moderated FHR. There is also an available recommended
13-group structure commented out in the `gentryGroups.serp` file 
The PDF of these findings can be found [here](http://trace.tennessee.edu/utk_graddiss/3695/).

The "fuel" file contains "cards" (a relic term referring to the fortran-dominated reactor physics
ages of yore) that will generate group constants at a few temperatures, and likewise for the
"moder" file. Like a cooking show, we have prepared the important results for you in the
`fuel.coe` and `moder.coe` files, which get parsed by `serpentTools` in the `extractSerpent2GCs`
script. `serpentTools` is a suite of parsers designed by GATech for parsing `SERPENT` output files.
More information can be found at [here](http://serpent-tools.readthedocs.io/en/latest/).

### Step 2: Parsing Serpent 2 Output Files with extractSerpent2GCs.py

The command to run in order to generate the Moltres-compatible group constants is:

```
$MOLTRES/python/extractSerpent2GCs.py MSREProperties msre_gentry_4g tempMapping.txt secBranch.txt universeMapping.txt
```

where $MOLTRES is an environment variable leading to the install location of Moltres. An
alternative would be to just add the $MOLTRES/python directory to your path.

The input syntax requires a directory name you'd like to create, a file base name that Moltres
will look at, a file that maps primary branch names to temperatures, a file that lists the
secondary branch names, and lastly a file that maps universe numbers from Serpent to material
names. Group constants will be extracted for all materials listed in the last file.

The ```secBranch.txt``` file should be blank if no secondary branches were used, i.e. there is only
one branch variation per burnup step.
