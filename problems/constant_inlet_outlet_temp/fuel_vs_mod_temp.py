import yt

def _unity(field, data):
    return 1.

ds = yt.load('./2group.e', step=-1)
ds.add_field(('all', 'temp'), function=_unity, units="K", force_override=True, take_log=False)

lines = []
lines.append(yt.LineBuffer(ds, (0, 0, 0), (0, 162.56, 0), 1000, label='fuel'))
lines.append(yt.LineBuffer(ds, (3, 0, 0), (3, 162.56, 0), 1000, label='graphite'))
plt = yt.LinePlot.from_lines(ds, [('all', 'temp')], lines, field_labels={('all', 'temp') : ''})
plt.annotate_legend(('all', 'temp'))
plt.set_x_unit('cm')
plt.set_x_unit('inch')
plt.set_unit(('all', 'temp'), 'degF')
plt.set_xlabel('z (in)')
plt.set_ylabel('Temperature (F)')
plt.save("fuel_vs_moderator_axial_sweep.eps")
