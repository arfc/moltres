
#ifndef ELEMENTL2DIFF_H
#define ELEMENTL2DIFF_H

#include "ElementIntegralVariablePostprocessor.h"

// Forward Declarations
class ElementL2Diff;

template <>
InputParameters validParams<ElementL2Diff>();

class ElementL2Diff : public ElementIntegralVariablePostprocessor
{
public:
  ElementL2Diff(const InputParameters & parameters);

protected:
  /**
   * Get the L2 Error.
   */
  virtual Real getValue();

  virtual Real computeQpIntegral();

  const VariableValue & _u_old;
};

#endif // ELEMENTL2DIFF_H
