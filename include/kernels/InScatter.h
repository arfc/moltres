#ifndef INSCATTER_H
#define INSCATTER_H

#include "Kernel.h"
#include "MaterialProperty.h"

// Forward Declaration
class InScatter;

template<>
InputParameters validParams<InScatter>();


class InScatter : public Kernel
{
public:
  InScatter(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<Real> & _sigma_s;
  const VariableValue & _v;
  unsigned int _v_id;
};

#endif //INSCATTER_H
