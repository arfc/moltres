#pragma once

#include "ArrayKernel.h"
#include "MoltresUtils.h"

class SNStreaming : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNStreaming(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  const MaterialProperty<Real> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _sigma_t;
  const MaterialProperty<std::vector<Real>> & _d_sigma_t_d_temp;
  unsigned int _N;
  unsigned int _group;
  unsigned int _temp_id;
  RealEigenMatrix _ordinates;
};
