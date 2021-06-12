#pragma once

#include "EigenKernel.h"

class SelfFissionEigenKernel : public EigenKernel
{
public:
  SelfFissionEigenKernel(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const MaterialProperty<Real> & _nu_f;
  const MaterialProperty<Real> & _sigma_f;
};
