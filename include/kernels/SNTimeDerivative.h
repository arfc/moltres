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

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _recipvel;
  const MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;

  /// Neutron group number
  const unsigned int _group;

  /// Temperature variable ID
  const unsigned int _temp_id;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
