#pragma once

#include "VariableNotAMooseObjectAction.h"

/**
 * This Action adds all required delayed neutron precursor variables and
 * kernels to the problem. Since the delayed neutron precursors transport
 * problem is pure advection, the problem is solved using discontinous Galerkin
 * as documented in:
 * <a href="http://epubs.siam.org/doi/book/10.1137/1.9780898717440">
 * Discontinuous Galerkin Methods for Solving Elliptic and Parabolic
 * Equations: Theory and Implementation
 * </a>
 * Note that the flow velocity must be constant when using the DGAdvection
 * or DGTemperatureAdvection. In order to vary to flow through a user-defined
 * function, use DGFunctionAdvection or DGFunctionTemperatureAdvection.
 */
class PrecursorAction : public VariableNotAMooseObjectAction
{
public:
  PrecursorAction(const InputParameters & params);

  static InputParameters validParams();

  virtual void act() override;

protected:
  using Action::addRelationshipManagers;
  void addRelationshipManagers(Moose::RelationshipManagerType when_type) override;

  /**
  * Adds PrecursorSource kernel
  *
  * @param op The zero-based index for the precursor group the kernel acts on
  * @param var_name The name of the variable the kernel acts on
  */
  void addPrecursorSource(const unsigned & op, const std::string & var_name);

  /**
  * Adds PrecursorDecay kernel
  *
  * @param op The zero-based index for the precursor group the kernel acts on
  * @param var_name The name of the variable the kernel acts on
  */
  void addPrecursorDecay(const unsigned & op, const std::string & var_name);

  /**
  * Adds ScalarTransportTimeDerivative kernel
  *
  * @param var_name The name of the variable the kernel acts on
  */
  void addTimeDerivative(const std::string & var_name);

  /**
  * Adds appropriate conservative advection kernel
  *
  * @param var_name The name of the variable the kernel acts on
  */
  void addAdvection(const std::string & var_name);

  /**
  * Adds DGAdvection kernel
  *
  * @param var_name The name of the variable the kernel acts on
  */
  void addDGAdvection(const std::string & var_name);

  /**
  * Adds appropriate outflow BC depending on velocity type
  *
  * @param var_name The name of the variable the BC acts on
  */
  void addOutflowBC(const std::string & var_name);

  /**
  * Adds appropriate inflow BC depending on velocity type
  *
  * @param var_name The name of the variable the BC acts on
  */
  void addInflowBC(const std::string & var_name);

  /**
   * Adds a PostprocessorPenaltyDirichletBC to stabilize precursor conc at inlet
   */
  void addPenaltyBC(const std::string & var_name);

  /**
  * Adds random initial conditions for Jacobian testing
  *
  * @param var_name The name of the variable the IC acts on
  */
  void addInitialConditions(const std::string & var_name);

  /**
  * Adds postprocessors to calculate flow-weighted precursor conc at outlet
  *
  * @param var_name The name of the variable the postprocessor acts on
  */
  void addOutletPostprocessor(const std::string & var_name);

  /**
  * Adds Receiver postprocessor required for precursor conc at inlet
  *
  * @param var_name The name of the variable the postprocessor acts on
  */
  void addInletPostprocessor(const std::string & var_name);

  /**
  * Adds MultiAppTransfers to simulate precursor looped flow by exchanging inlet/outlet data
  *
  * @param var_name The name of the variable the MultiAppTransfers acts on
  */
  void addMultiAppTransfer(const std::string & var_name);

  /**
  * Adds postprocessor to calculate coolant outflow rate required by addOutletPostprocessor
  *
  * @param var_name The name of the variable the postprocessor acts on
  */
  void addCoolantOutflowPostprocessor();

  /**
  * Sets variable and block params. Used to reduce repeating code
  *
  * @param params The InputParameters in which to set variable and block params
  * @param var_name The name of the variable the postprocessor acts on
  */
  void setVarNameAndBlock(InputParameters & params, const std::string & var_name);

  /// number of precursor groups
  unsigned int _num_precursor_groups;

  /// base name for neutron variables
  std::string _var_name_base;

  /// number of energy groups
  unsigned int _num_groups;

  /// optional object name suffix
  std::string _object_suffix;

  /// whether input file is for the outer loop
  bool _is_loopapp;

  /// velocity type
  MooseEnum _velocity_type;
};
