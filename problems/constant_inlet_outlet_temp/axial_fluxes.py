import yt

field_label = {('all', 'group1') : r'$\phi_1$', ('all', 'group2') : r'$\phi_2$'}

ds = yt.load('./2group.e', step=-1)

plt = yt.LinePlot(ds, [('all', 'group1'), ('all', 'group2')], [0, 0, 0], [0, 162.56, 0], 1000, field_labels=field_label)
plt.annotate_legend(('all', 'group1'))
plt.set_x_unit('cm')
plt.set_x_unit('inch')
plt.set_xlabel('z (in)')
plt.set_ylabel(r'Fluxes ($\mathrm{cm^{-2} s^{-1}}$)')
plt.save("moltres_axial_fluxes.eps")
