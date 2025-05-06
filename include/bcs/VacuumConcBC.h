#pragma once

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

/**
 * Implements a simple VacuumConc BC for neutron diffusion on vacuum boundaries.
 * VacuumConc BC is defined as \f$ D\frac{du}{dn}+\frac{u}{\alpha} = 0\f$, where u is neutron flux.
 * Hence, \f$ D\frac{du}{dn}=-\frac{u}{\alpha} \f$ and \f$ -\frac{u}{\alpha} \f$ is substituted
 * into the Neumann BC term produced from integrating the diffusion operator by parts.
 *
 * The three types of vacuum BCs available are Marshak ($\alpha=2$), Mark ($\alpha=\sqrt{3}$), and
 * Milne ($\alpha=3\times 0.710446$). VacuumConcBC defaults to Marshak if `bc_type` is not set.
 */
class VacuumConcBC : public IntegratedBC, public ScalarTransportBase
{
public:
  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  VacuumConcBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  enum BC_TYPE
  {
    MARSHAK,
    MARK,
    MILNE
  };

  // Ratio of u to du/dn
  Real _alpha;

  // Milne vacuum boundary extrapolation coefficient
  // Derived from the exact analytical solution to the Milne problem. See MooseDocs-based
  // documentation for more information.
  Real _milne_extrapolation_coefficient = 3 * 0.710446;
};
