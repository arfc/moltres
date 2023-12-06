#pragma once

#include "Moose.h"
#include "MooseTypes.h"
#include "MathUtils.h"

namespace MoltresUtils
{

std::vector<Real> points(unsigned int N);
RealEigenMatrix permute_octant(
    RealEigenMatrix m, unsigned int idx, Real mu1, Real mu2, Real mu3, Real w);
RealEigenMatrix level_symmetric(unsigned int N);
unsigned int x_reflection_map(unsigned int i);
unsigned int y_reflection_map(unsigned int i);
unsigned int z_reflection_map(unsigned int i);
std::vector<Real> weights(unsigned int N);
Real sph_harmonics(int l, int m, Real eta, Real xi, Real mu);
int factorial(int n);

} // namespace MoltresUtils
