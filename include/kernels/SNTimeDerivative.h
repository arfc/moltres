#pragma once

#include "ArrayTimeKernel.h"

class SNTimeDerivative : public ArrayTimeKernel
{
public:
  static InputParameters validParams();

  SNTimeDerivative(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _recipvel;
  const MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;
  const unsigned int _N;
  const unsigned int _group;
  const unsigned int _temp_id;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
