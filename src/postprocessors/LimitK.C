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

#include "LimitK.h"
#include "FEProblem.h"

registerMooseObject("MoltresApp", LimitK);

InputParameters
LimitK::validParams()
{
  InputParameters params = TimestepSize::validParams();
  params.addRequiredParam<Real>("growth_factor",
                                "How much to grow the time step if multiplication is ok.");
  params.addRequiredParam<Real>("cutback_factor",
                                "How much to cut the time step if multiplication is too high.");
  params.addRequiredParam<Real>("k_threshold",
                                "The maximum k value before we start cutting time steps.");
  params.addRequiredParam<PostprocessorName>("k_postprocessor",
                                             "The name of the postprocessor providing k values.");
  return params;
}

LimitK::LimitK(const InputParameters & parameters)
  : TimestepSize(parameters),
    _k(getPostprocessorValue("k_postprocessor")),
    _growth_factor(getParam<Real>("growth_factor")),
    _cutback_factor(getParam<Real>("cutback_factor")),
    _k_threshold(getParam<Real>("k_threshold"))
{
}

Real
LimitK::getValue() const
{
  Real dt = _feproblem.dt();
  if (_k > _k_threshold)
    return dt * _cutback_factor;
  else
    return dt * _growth_factor;
}
