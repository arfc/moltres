#!/usr/bin/env python
#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

from matplotlib import pyplot
import numpy

x = numpy.linspace(0, 6, num=100)
y = lambda x: 8. - 29./6.*x + 5./6.*x*x
fig = pyplot.figure(figsize=(6,3), dpi=300, tight_layout=True)
ax = fig.add_subplot(111, ylim=[0,8])
ax.grid(True, color=[0.7]*3)
ax.plot(x, y(x))
fig.savefig('fem_example.png')
