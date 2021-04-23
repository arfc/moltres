#pragma once

#include "VariableNotAMooseObjectAction.h"

/**
 * Add neutronics kernels and variables to MSR simulations automatically.
 * When writing the multigroup diffusion equation:
 * \f[
 *   \frac{1}{v_g}\frac{\partial {\phi}_g}{\partial t}   = \nabla \cdot D_g
                \nabla {\phi}_g +
                \sum_{g \ne g'}^G
                {\Sigma_{g'\rightarrow g}^s} {\phi}_{g'} + \chi_g^p \sum_{g' = 1}^G (1 -
                \beta) \nu {\Sigma_{g'}^f} {\phi}_{g'} + \chi_g^d \sum_i^I
                \lambda_i {C_i} - {\Sigma_g^r} {\phi_g}
 * \f]
 * it's clearly the case that many terms are involved. Moltres, and MOOSE in general represent
 * each term as a "kernel". This action adds all of the required kernels for this problem to the
 * simulation. In addition, when using many flux variables, it adds the required variables to the
 * problem as well.
 */
class NtAction : public VariableNotAMooseObjectAction
{
public:
  NtAction(const InputParameters & params);

  static InputParameters validParams();

protected:
  void act() override;

  /// number of precursor groups
  unsigned int _num_precursor_groups;

  /// base name for neutron variables
  std::string _var_name_base;

  /// number of energy groups
  unsigned int _num_groups;
};
