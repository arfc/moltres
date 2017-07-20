#ifndef GAMMAEHEATSOURCE_H
#define GAMMAEHEATSOURCE_H

#include "Kernel.h"

// Forward Declarations
class GammaHeatSource;

template <>
InputParameters validParams<GammaHeatSource>();

class GammaHeatSource : public Kernel
{
public:
  GammaHeatSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const PostprocessorValue & _average_fission_heat;
  Function & _gamma;
};

#endif // GAMMAEHEATSOURCE_H
