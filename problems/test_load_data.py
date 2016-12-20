import yt
import netCDF4

# import pdb; pdb.set_trace()
ncdf_ds = netCDF4.Dataset('/home/lindsayad/projects/moltres/problems/two-region-example_out.e')
print(ncdf_ds.variables['vals_nod_var1'][:])
print(ncdf_ds.variables['vals_nod_var1'][:].shape)
ds = yt.load("two-region-example_out.e", step=-1)
ad = ds.all_data()
print(ad['diffused'])
print(ad['diffused'].shape)
print(ad[('connect1', 'diffused')])
