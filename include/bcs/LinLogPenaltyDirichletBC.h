#pragma once

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

class LinLogPenaltyDirichletBC : public IntegratedBC, public ScalarTransportBase
{
public:
  LinLogPenaltyDirichletBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

private:
  Real _p;
  Real _v;
};
