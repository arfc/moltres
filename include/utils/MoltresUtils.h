#pragma once

#include "Moose.h"
#include "MooseError.h"
#include "MathUtils.h"

namespace MoltresUtils
{

std::vector<Real> ordinates(unsigned int N);
std::vector<Real> weights(unsigned int N);
Real sph_harmonics(unsigned int l, int m, Real eta, Real xi, Real mu);
unsigned int factorial(unsigned int n);

} // namespace MoltresUtils
