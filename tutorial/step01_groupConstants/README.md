## Group Constants

Here, group constants are taken from example Serpent output using PyNE. These will be used to generate a properties directory, which
can be used by moltres. See extractSerpent2GCs.py for more.

Moltres is *not* a spatially homogenized diffusion code. MOOSE is made for running, **big**, **intense** problems, using modern HPC.
Because there is no desire to homogenize spatially here, the materials that the user would like used in moltres should each fill their
own infinite universe. Then, this universe should have its group constants generated using the "set gcu <material universe numbers>".

It's important that universe 0, the main universe, is not included. Serpent takes tallies for group constants in the first universe
it identifies, so including 0 means that no further universes will be included in GC generation. (please double check this on the serpent
forum).

### Serpent Input

Here, serpent input can be found to simulate ORNL's molten salt reactor experiment, documented largely on websites like
(this)[http://www.energyfromthorium.com/pdf/] one.

For group constant generation, Cole Gentry found that a 4 group structure with three thermal groups and one fast group works
well in the graphite moderated FHR. There is also an available recommended 13 group structure, which could be found by changing
some comment characters around in the include file defining group structures. The PDF of these findings can be found (here)[http://trace.tennessee.edu/utk_graddiss/3695/].

The "fuel" file contains "cards" (a relic term referring to the fortran-dominated reactor physics ages of yore)
that will generate group constants at a few temperatures, and likewise for the "moder" file.
Like a cooking show, the important results have already been prepared for you in the "fuel.coe" and "moder.coe" files, which
get parsed by PyNE in the extractSerpent2GCs script.

### Command to run:

The command to run in order to generate the moltres-compatible group constants is:

```./extractSerpent2GCs.py MSREProperties msre_gentry_4g tempMapping.txt universeMapping.txt fuel moder```

The syntax requires the arbitrary directory name you'd like to create, then the arbitrary file base name that moltres will
look at, then a file that maps branch names to temperatures, then a file that maps universe numbers from serpent to material names,
and lastly the list of all materials you'd like to pass group constants into moltres for.
