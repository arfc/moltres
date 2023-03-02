import openmc
import sys
sys.path.insert(1, '../')
from moltres_xs import openmc_xs  # noqa: E402

fuel_mat = openmc.Material(name='fuel', material_id=1)
fuel_mat.set_density('atom/b-cm', 4.7984E-02)
fuel_mat.temperature = 900
fuel_mat.add_nuclide("U235", 4.4994E-02, percent_type='ao')
fuel_mat.add_nuclide("U238", 2.4984E-03, percent_type='ao')
fuel_mat.add_nuclide("U234", 4.9184E-04, percent_type='ao')

mats = openmc.Materials([fuel_mat])
mats.export_to_xml()

sph = openmc.Sphere(r=8.7407, boundary_type='vacuum')

fuel_core = openmc.Cell(fill=fuel_mat, region=-sph)

geom = openmc.Geometry([fuel_core])
geom.export_to_xml()

settings = openmc.Settings()
settings.batches = 100
settings.inactive = 20
settings.particles = 50000
settings.temperature = {"multipole": True, "method": "interpolation"}
point = openmc.stats.Point((0, 0, 0))
src = openmc.Source(space=point)
settings.source = src
settings.export_to_xml()

tallies_file = openmc.Tallies()
domain_dict = openmc_xs.generate_openmc_tallies_xml(
    [1e-5, 748.5, 5.5308e3, 24.7875e3, 0.4979e6, 2.2313e6, 20e6],
    list(range(1, 9)),
    [fuel_mat],
    [fuel_mat.id],
    tallies_file,
)
