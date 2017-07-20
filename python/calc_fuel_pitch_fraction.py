import sympy as sp
import sys


def calc_spacing(R, n):
    x = sp.symbols('x')
    Af = 0
    Am = 0
    for m in range(2*n - 1):
        if m % 2 == 0:
            Af += sp.pi * (R/n * (m/2 + x))**2
        if m % 2 == 1:
            Af -= sp.pi * (R/n * (m+1)/2)**2
    for m in range(2*n):
        if m % 2 == 0:
            Am -= sp.pi * (R/n * (m/2 + x))**2
        if m % 2 == 1:
            Am += sp.pi * (R/n * (m+1)/2)**2
    return sp.solve(Af / (Af + Am) - .225, x)


print(calc_spacing(float(sys.argv[1]), int(sys.argv[2])))
