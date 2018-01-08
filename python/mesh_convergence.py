import matplotlib.pyplot as plt

home = "/Users/lindad"
refine = [0, 1, 2, 3]
g1_flux = [4.62, 5.75, 6.06, 6.14]

plt.plot(refine, g1_flux, linestyle='none',marker='o')
plt.xlabel("Refinement level")
plt.ylabel("Integrated fast group flux ($10^{19}$ cm/s)")
plt.xticks([0, 1, 2, 3])
# plt.show()
plt.savefig(home + "/publications/figures/mesh_convergence.eps")
