import yt
import numpy as np
from yt.units import K

home = "/Users/lindad"

def _units(field, data):
    return data[('all', 'temp')] * K


ds = yt.load(
    home + '/projects/moltres/problems/'
    '2017_annals_pub_msre_compare/2group_out.e', step=-1)
ds.add_field(('all', 'temp_K'), units='K', function=_units, take_log=False)
lines = []
lines.append(yt.LineBuffer(
    ds, (0, 0, 0), (0, 162.56, 0), 1000, label='fuel moltres'))
lines.append(yt.LineBuffer(ds, (3, 0, 0), (3, 162.56, 0),
                           1000, label='graphite moltres'))
plt = yt.LinePlot.from_lines(ds, [('all', 'temp_K')], lines, field_labels={
                             ('all', 'temp_K'): ''})
plt.annotate_legend(('all', 'temp_K'))
plt.set_x_unit('cm')
plt.set_xlabel('z (cm)')
plt.set_ylabel('Temperature (K)')
plt._setup_plots()

msre_temp_axial_data = np.loadtxt(
    home + '/publications/figures/'
    'msre_axial_temps.csv', skiprows=1, delimiter=',')
x = msre_temp_axial_data[:, 0] * 2.54
fuel_temp = (msre_temp_axial_data[:, 1] - 32) * 5. / 9. + 273.15
graphite_temp = (msre_temp_axial_data[:, 2] - 32) * 5. / 9. + 273.15
plt.plots[('all', 'temp_K')].axes.plot(x, fuel_temp, label=r'fuel msre')
plt.plots[('all', 'temp_K')].axes.plot(
    x, graphite_temp, label=r'graphite msre')
plt.plots[('all', 'temp_K')].axes.legend()
plt._setup_plots()

plt.save(home + '/publications/figures/'
         'combined_msre_moltres_axial_temps.eps')
