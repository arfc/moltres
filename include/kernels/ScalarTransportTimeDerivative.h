#ifndef SCALARTRANSPORTTIMEDERIVATIVE_H
#define SCALARTRANSPORTTIMEDERIVATIVE_H

#include "TimeKernel.h"
#include "ScalarTransportBase.h"

// Forward Declaration
class ScalarTransportTimeDerivative;

template<>
InputParameters validParams<ScalarTransportTimeDerivative>();

class ScalarTransportTimeDerivative : public ScalarTransportBase<TimeKernel>
{
public:
  ScalarTransportTimeDerivative(const InputParameters & parameters);


protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  bool _lumping;
};

#endif //SCALARTRANSPORTTIMEDERIVATIVE_H
