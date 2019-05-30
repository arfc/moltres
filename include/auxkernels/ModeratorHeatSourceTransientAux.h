#ifndef MODERATORHEATSOURCETRANSIENTAUX_H
#define MODERATORHEATSOURCETRANSIENTAUX_H

#include "AuxKernel.h"

// Forward Declarations
class ModeratorHeatSourceTransientAux;

template <>
InputParameters validParams<ModeratorHeatSourceTransientAux>();

/**
 * When a reactor runs, gamma rays are emitted in extraordinary quantity.
 * These gammas tend to heat the graphite in thermal MSRs, as experienced at the MSRE,
 * and make the graphite's steady temperature quite a bit hotter than then the salt.
 * ORNL found that gamma heating in graphite is nearly uniform over the core since gammas have
 * a long mean free path. In addition, the gamma heating is proportional to the average fission
 * heat. This kernel computes local gamma heating as a function of average fission heat and a
 * user-defined proportionality factor (usually between 2 and 10 percent).
 *
 * Gamma can define a form factor for the gamma heating. That is, gamma heating can be set to be
 * cosinusoidal or Bessel.
 */
class ModeratorHeatSourceTransientAux : public AuxKernel
{
public:
  ModeratorHeatSourceTransientAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const PostprocessorValue & _average_fission_heat;
  const Function & _gamma;
};

#endif // MODERATORHEATSOURCETRANSIENTAUX_H
