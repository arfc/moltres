#ifndef MATDIFFUSIONFLUXBC_H
#define MATDIFFUSIONFLUXBC_H

#include "IntegratedBC.h"

class MatDiffusionFluxBC;

template<>
InputParameters validParams<MatDiffusionFluxBC>();

class MatDiffusionFluxBC : public IntegratedBC
{
public:
  MatDiffusionFluxBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  const MaterialProperty<Real> * _diff;
};

#endif /* MATDIFFUSIONFLUXBC_H */
