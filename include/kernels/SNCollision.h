#pragma once

#include "ArrayKernel.h"

class SNCollision : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNCollision(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  const MaterialProperty<std::vector<Real>> & _sigma_t;
  const MaterialProperty<std::vector<Real>> & _d_sigma_t_d_temp;
  unsigned int _N;
  unsigned int _group;
  unsigned int _temp_id;
};
