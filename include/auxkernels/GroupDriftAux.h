#pragma once

#include "AuxKernel.h"
#include "ScalarTransportBase.h"

class GroupDriftAux : public ArrayAuxKernel, public ScalarTransportBase
{
public:
  static InputParameters validParams();

  GroupDriftAux(const InputParameters & parameters);

protected:
  virtual RealEigenVector computeValue() override;

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _diffcoef;
  const MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;
  const MaterialProperty<std::vector<Real>> & _totxs;
  const MaterialProperty<std::vector<Real>> & _d_totxs_d_temp;
  const MaterialProperty<std::vector<Real>> & _scatter;
  const MaterialProperty<std::vector<Real>> & _d_scatter_d_temp;
  const unsigned int _N;
  const unsigned int _group;
  const unsigned int _num_groups;
  const unsigned int _temp_id;
  std::vector<const ArrayVariableValue *> _group_fluxes;
  std::vector<const ArrayVariableGradient *> _grad_group_fluxes;
  std::vector<unsigned int> _flux_ids;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
