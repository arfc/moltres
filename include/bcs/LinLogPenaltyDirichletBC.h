#ifndef LINLOGPENALTYDIRICHLETBC_H
#define LINLOGPENALTYDIRICHLETBC_H

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

class LinLogPenaltyDirichletBC;

template <>
InputParameters validParams<LinLogPenaltyDirichletBC>();

class LinLogPenaltyDirichletBC : public IntegratedBC, public ScalarTransportBase
{
public:
  LinLogPenaltyDirichletBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

private:
  Real _p;
  Real _v;
};

#endif
