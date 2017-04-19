#ifndef MATDIFFUSIONAUX_H
#define MATDIFFUSIONAUX_H

#include "AuxKernel.h"
#include "MaterialProperty.h"

// Forward Declaration
class MatDiffusionAux;

template <>
InputParameters validParams<MatDiffusionAux>();

class MatDiffusionAux : public AuxKernel
{
public:
  MatDiffusionAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  std::string _prop_name;
  const VariableGradient & _grad_diffuse_var;
  const MaterialProperty<Real> * _diff;
};

#endif // MATDIFFUSIONAUX_H
