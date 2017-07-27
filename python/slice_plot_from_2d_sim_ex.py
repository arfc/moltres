import yt
from moltres_functions import actual_center_and_widths

field_label = {'group1' : r'$\phi_1\cdot$10$^{13}$ cm$^{-2}$ s$^{-1}$', 'group2' : r'$\phi_2\cdot$10$^{13}$ cm$^{-2}$ s$^{-1}$',
               'temp' : 'T (K)', 'pre1' : r'C$_1$ cm$^{-3}$', 'pre2' : r'C$_2$ cm$^{-3}$', 'pre3' : r'C$_3$ cm$^{-3}$',
               'pre4' : r'C$_4$ cm$^{-3}$', 'pre5' : r'C$_5$ cm$^{-3}$', 'pre6' : r'C$_6$ cm$^{-3}$'}
names = [name for name in field_label.keys() if 'pre' in name]
for name in names:
    field_label[name + '_scaled'] = field_label[name]

def _scale(field, data):
    new_field = field.name
    ftype, fname = new_field
    original_fname = fname.split('_')[0]
    return 1e13 * data[('all', original_fname)]

ds = yt.load('/home/lindsayad/projects/moltres/tests/twod_axi_coupled/auto_diff_rho.e', step=-1)
for i in range(1, 7):
    fname = 'pre%d_scaled' % i
    ds.add_field(('all', fname), function=_scale, take_log=False)
actual_domain_widths, actual_center = actual_center_and_widths(ds)

for field in ds.field_info.keys():
    field_type, field_name = field
    if field_type != 'all':
        continue
    if 'scaled' not in field_name:
        continue
    slc = yt.SlicePlot(ds, 'z', field, origin='native', center=actual_center)
    slc.set_log(field, False)
    slc.set_width((actual_domain_widths[0],actual_domain_widths[1]))
    slc.set_xlabel("r (cm)")
    slc.set_ylabel("z (cm)")
    if field_name == 'temp':
        slc.set_zlim(field, 922, ds.all_data()[('all','temp')].max())
    slc.set_colorbar_label(field, field_label[field_name])
    slc.set_figure_size(3)
    slc.save('/home/lindsayad/publications/figures/2d_gamma_heating_' + field_name + '.eps')
