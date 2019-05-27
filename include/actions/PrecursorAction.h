#ifndef PRECURSORACTION_H
#define PRECURSORACTION_H

#include "AddVariableAction.h"

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
class PrecursorAction : public AddVariableAction
{
public:
  PrecursorAction(const InputParameters & params);

  void act() override;

  using Action::addRelationshipManagers;
  void addRelationshipManagers(Moose::RelationshipManagerType when_type) override;

protected:
  virtual void kernelAct(const unsigned & op, const std::string & var_name);
  virtual void bcAct(const std::string & var_name);
  virtual void dgKernelAct(const std::string & var_name);
  virtual void icAct(const std::string & var_name);
  virtual void postAct(const std::string & var_name);
  virtual void transferAct(const std::string & var_name);

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
