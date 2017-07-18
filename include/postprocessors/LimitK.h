/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef LIMITK_H
#define LIMITK_H

#include "TimestepSize.h"

// Forward Declarations
class LimitK;

template <>
InputParameters validParams<LimitK>();

class LimitK : public TimestepSize
{
public:
  LimitK(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override {}

  /**
   * This will return the current time step size.
   */
  virtual Real getValue() override;

protected:
  const PostprocessorValue & _k;
  const Real & _growth_factor;
  const Real & _cutback_factor;
  const Real & _k_threshold;
};

#endif // LIMITK_H
