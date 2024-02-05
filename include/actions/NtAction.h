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

  virtual void act() override;

protected:
  /// number of precursor groups
  unsigned int _num_precursor_groups;

  /// base name for neutron variables
  std::string _var_name_base;

  /// number of energy groups
  unsigned int _num_groups;

  /**
  * Adds non-source neutronics kernel
  *
  * @param op The zero-based index for the precursor group the kernel acts on
  * @param var_name The name of the variable the kernel acts on
  * @param kernel_type The kernel type to be added
  * @param all_var_names Vector of the names of all group flux variables
  */
  void addNtKernel(const unsigned & op,
                   const std::string & var_name,
                   const std::string & kernel_type,
                   const std::vector<VariableName> & all_var_names);

  /**
  * Adds CoupledFissionKernel kernel
  *
  * @param op The zero-based index for the precursor group the kernel acts on
  * @param var_name The name of the variable the kernel acts on
  * @param all_var_names Vector of the names of all group flux variables
  */
  void addCoupledFissionKernel(const unsigned & op,
                               const std::string & var_name,
                               const std::vector<VariableName> & all_var_names);

  /**
  * Adds DelayedNeutronSource kernel
  *
  * @param op The zero-based index for the precursor group the kernel acts on
  * @param var_name The name of the variable the kernel acts on
  */
  void addDelayedNeutronSource(const unsigned & op, const std::string & var_name);
};
