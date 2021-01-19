#ifndef PRECURSORACTION_H
#define PRECURSORACTION_H

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

  void act() override;

  using Action::addRelationshipManagers;
  void addRelationshipManagers(Moose::RelationshipManagerType when_type) override;

protected:
  /// Adds PrecursorSource kernel
  void addPrecursorSource(const unsigned & op, const std::string & var_name);
  /// Adds PrecursorDecay kernel
  void addPrecursorDecay(const unsigned & op, const std::string & var_name);
  /// Adds ScalarTransportTimeDerivative kernel
  void addTimeDerivative(const std::string & var_name);
  /// Adds DGAdvection kernel
  void addDGAdvection(const std::string & var_name);
  /// Adds appropriate outflow BC depending on velocity type
  void addOutflowBC(const std::string & var_name);
  /// Adds appropriate inflow BC depending on velocity type
  void addInflowBC(const std::string & var_name);
  /// Adds random initial conditions for Jacobian testing
  void addInitialConditions(const std::string & var_name);
  /// Adds postprocessors to calculate flow-weighted precursor conc at outlet
  void addOutletPostprocessor(const std::string & var_name);
  /// Adds Receiver postprocessor required for precursor conc at inlet
  void addInletPostprocessor(const std::string & var_name);
  /// Adds MultiAppTransfers to simulate precursor looped flow by exchanging inlet/outlet data
  void addMultiAppTransfer(const std::string & var_name);
  /// Adds postprocessor to calculate coolant outflow rate required by addOutletPostprocessor
  void addCoolantOutflowPostprocessor();
  /// Sets variable and block params. Used to reduce repeating code
  void setVarNameAndBlock(InputParameters & params, const std::string & var_name);

  /// number of precursor groups
  unsigned int _num_precursor_groups;

  /// base name for neutron variables
  std::string _var_name_base;

  /// number of energy groups
  unsigned int _num_groups;

  /// optional object name suffix
  std::string _object_suffix;
};

template <>
InputParameters validParams<PrecursorAction>();

#endif // PRECURSORACTION_H
