#ifndef DIVISIONPOSTPROCESSOR_H
#define DIVISIONPOSTPROCESSOR_H

#include "DifferencePostprocessor.h"

class DivisionPostprocessor;

template<>
InputParameters validParams<DivisionPostprocessor>();

/**
 * Computes the quotient between two postprocessors
 *
 * result = value1 / value2
 */
class DivisionPostprocessor : public DifferencePostprocessor
{
public:
  DivisionPostprocessor(const InputParameters & parameters);

  virtual PostprocessorValue getValue() override;
};


#endif /* DIVISIONPOSTPROCESSOR_H */
