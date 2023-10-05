#pragma once

#include "Moose.h"
#include "MooseTypes.h"
#include "MooseError.h"
#include "MathUtils.h"

namespace MoltresUtils
{

std::vector<Real> points(unsigned int N);
RealEigenMatrix directions(unsigned int N);
std::vector<Real> weights(unsigned int N);
Real sph_harmonics(unsigned int l, int m, Real eta, Real xi, Real mu);
int factorial(int n);

} // namespace MoltresUtils
