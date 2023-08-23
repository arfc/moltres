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

#pragma once

#include "TimestepSize.h"

class LimitK : public TimestepSize
{
public:
  LimitK(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void initialize() override {}
  virtual void execute() override {}

  using Postprocessor::getValue;
  virtual Real getValue() const override;

protected:
  const PostprocessorValue & _k;
  const Real & _growth_factor;
  const Real & _cutback_factor;
  const Real & _k_threshold;
};
