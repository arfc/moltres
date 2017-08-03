import sympy as sp
import sys


def calc_spacing(R, n):
    '''
    This function takes the total reactor radius, R, and
    the number of channel-moderator segments, n, and outputs
    the fraction of the pitch that should be fuel in order
    to realize an overall fuel volume fraction of .225. The
    resulting (positive) value corresponds to 'x' in, for example,
    `tests/twod_axi_coupled/2d_lattice_structured.geo`
    '''
    x = sp.symbols('x')
    Af = 0
    Am = 0
    for m in range(2 * n - 1):
        if m % 2 == 0:
            Af += sp.pi * (R / n * (m / 2 + x))**2
        if m % 2 == 1:
            Af -= sp.pi * (R / n * (m + 1) / 2)**2
    for m in range(2 * n):
        if m % 2 == 0:
            Am -= sp.pi * (R / n * (m / 2 + x))**2
        if m % 2 == 1:
            Am += sp.pi * (R / n * (m + 1) / 2)**2
    return sp.solve(Af / (Af + Am) - .225, x)


print(calc_spacing(float(sys.argv[1]), int(sys.argv[2])))
