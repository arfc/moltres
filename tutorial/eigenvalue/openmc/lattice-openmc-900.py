import openmc
import sys
import os
import matplotlib.pyplot as plt
import numpy as np
sys.path.insert(1, '/home/smpark/projects/moltres/python')
from moltres_xs import openmc_xs # noqa: E402

# %% Materials

fuel = openmc.Material(material_id=101)
# Fuel composition at initial criticality (U235 = 65.25kg)
fuel_dens = 2.3275
fuel.set_density('g/cm3', 2.3275)
fuel_elements = {'Li7': 507.27 * .99995,
                 'Li6': 507.27 * .00005,
                 'Be': 293.96,
                 'F': 3103.22,
                 'Zr': 513.97,
                 'U234': .67,
                 'U235': 65.25,
                 'U236': .27,
                 'U238': 141.91}
total_fuel_weight = sum(fuel_elements.values())
for i in fuel_elements.keys():
    if i == 'Zr' or i == 'F' or i == 'Be':
        fuel.add_element(i, fuel_elements[i]/total_fuel_weight, 'wo')
    else:
        fuel.add_nuclide(i, fuel_elements[i]/total_fuel_weight, 'wo')
fuel.add_s_alpha_beta('c_Be')
fuel.temperature = 900

graphite = openmc.Material(material_id=102)
graphite_dens = 1.86
graphite.set_density('g/cm3', graphite_dens)
graphite.add_nuclide('C0', 1, 'wo')
graphite.add_s_alpha_beta('c_Graphite')
graphite.temperature = 900

mats = openmc.Materials((fuel, graphite))

# %% Geometry

bot_plane = openmc.YPlane(y0=0, boundary_type="reflective")
top_plane = openmc.YPlane(y0=1, boundary_type="reflective")
left_plane = openmc.XPlane(x0=0, boundary_type="reflective")
right_plane = openmc.XPlane(x0=5, boundary_type="reflective")
itf1_plane = openmc.XPlane(x0=.625, boundary_type="transmission")
itf2_plane = openmc.XPlane(x0=4.325, boundary_type="transmission")

all_cells = []

left_fuel_cell = openmc.Cell(fill=fuel, cell_id=101)
left_fuel_cell.region = -top_plane & +bot_plane & +left_plane & -itf1_plane
all_cells.append(left_fuel_cell)

graphite_cell = openmc.Cell(fill=graphite, cell_id=102)
graphite_cell.region = -top_plane & +bot_plane & +itf1_plane & -itf2_plane
all_cells.append(graphite_cell)

right_fuel_cell = openmc.Cell(fill=fuel, cell_id=103)
right_fuel_cell.region = -top_plane & +bot_plane & +itf2_plane & -right_plane
all_cells.append(right_fuel_cell)

universe = openmc.Universe(cells=all_cells)

# %% Plot
universe.plot(origin=(2.5, 0.5, 0),
              width=(5, 1),
              color_by="material",
              colors={fuel: "red",
                      graphite: "grey"})

# %% settings
batches = 100
inactive = 20
particles = 5000

settings = openmc.Settings()
box = openmc.stats.Box((0, 0, 0), (5, 1, 0))
src = openmc.Source(space=box)
settings.source = src
settings.batches = batches
settings.inactive = inactive
settings.particles = particles
settings.output = {'tallies': False}
settings.temperature = {'multipole': True,
                        'method': 'interpolation',
                        'default': 900.}

# %% Moltres group constants
tallies_file = openmc.Tallies()
mats_id = []
for m in mats:
    mats_id.append(m.id)
domain_dict = openmc_xs.generate_openmc_tallies_xml(
    [1e-5, 1e0, 1e8],
    list(range(1, 7)),
    mats,
    mats_id,
    tallies_file,
)

# generate XML
mats.export_to_xml()

geom = openmc.Geometry(universe)
geom.export_to_xml()

settings.export_to_xml()
tallies_file.export_to_xml()
