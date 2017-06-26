#ifndef MODERATORHEATSOURCETRANSIENTAUX_H
#define MODERATORHEATSOURCETRANSIENTAUX_H

#include "AuxKernel.h"

// Forward Declarations
class ModeratorHeatSourceTransientAux;

template <>
InputParameters validParams<ModeratorHeatSourceTransientAux>();

class ModeratorHeatSourceTransientAux : public AuxKernel
{
public:
  ModeratorHeatSourceTransientAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const PostprocessorValue & _average_fission_heat;
  Real _gamma;
};

#endif // MODERATORHEATSOURCETRANSIENTAUX_H
