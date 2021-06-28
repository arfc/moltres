#pragma once

// Moose Includes
#include "DiracKernel.h"

/**
 * Provides a Dirac kernel for heating or cooling. At first,
 * this was used to model a heat exchanger in the reactor primary
 * loop. It was found that this didn't exactly provide the expected
 * amount of cooling, so InterfaceHX or InterfaceEffectivenessHX should
 * be used instead.
 *
 * This could still be a useful kernel for easily providing some point heat/
 * cooling sources.
 */
class DiracHX : public DiracKernel
{
public:
  DiracHX(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void addPoints() override;
  virtual Real computeQpResidual() override;

protected:
  const Real & _power;
  Point _point;
};
