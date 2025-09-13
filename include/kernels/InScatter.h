#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class InScatter : public Kernel, public ScalarTransportBase
{
public:
  InScatter(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _gtransfxs;
  const MaterialProperty<std::vector<Real>> & _d_gtransfxs_d_temp;

  /// Neutron group number
  const unsigned int _group;

  /// Number of neutron groups
  const unsigned int _num_groups;

  /// Temperature variable ID
  const unsigned int _temp_id;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  /// Group flux variable IDs
  std::vector<unsigned int> _flux_ids;

  /// Whether scattering matrix indexing follows Serpent 2 and OpenMC format
  const bool _sss2_input;

  /// Scattering matrix index
  std::vector<int> _idx;
};
