import yt
from moltres_functions import actual_center_and_widths

field_label = {'group1': r'$\phi_1\cdot$10$^{13}$ cm$^{-2}$ s$^{-1}$', 'group2': r'$\phi_2\cdot$10$^{13}$ cm$^{-2}$ s$^{-1}$',
               'temp': 'T (K)', 'pre1': r'C$_1$ cm$^{-3}$', 'pre2': r'C$_2$ cm$^{-3}$', 'pre3': r'C$_3$ cm$^{-3}$',
               'pre4': r'C$_4$ cm$^{-3}$', 'pre5': r'C$_5$ cm$^{-3}$', 'pre6': r'C$_6$ cm$^{-3}$'}

ds = yt.load(
    '/home/lindsayad/Dropbox/MSR_data/converged_gamma_heating_3d_steady_state/gamma_heating_newton.e', step=-1)
actual_domain_widths, actual_center = actual_center_and_widths(ds)

for field in ds.field_list:
    field_type, field_name = field
    import pdb
    pdb.set_trace()
    if field_type != 'all' or field_name != 'temp':
        continue
    slc = yt.SlicePlot(ds, 'z', field, origin='native', center=(
        actual_center[0], actual_center[1], actual_domain_widths[2]))
    slc.set_log(field, False)
    slc.set_width((actual_domain_widths[1], actual_domain_widths[2]))
    slc.set_xlabel("x (cm)")
    slc.set_ylabel("y (cm)")
    if field_name == 'temp':
        slc.set_zlim(field, 922, ds.all_data()[('all', 'temp')].max())
    slc.set_colorbar_label(field, field_label[field_name])
    slc.set_figure_size(3)
    slc.save('/home/lindsayad/publications/figures/3d_gamma_heating_z_slice_' +
             field_name + '.eps')
