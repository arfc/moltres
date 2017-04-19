#ifndef VACUUMCONCBC_H
#define VACUUMCONCBC_H

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class VacuumConcBC;

template <>
InputParameters validParams<VacuumConcBC>();

/**
 * Implements a simple VacuumConc BC for neutron diffusion on the boundary.
 * VacuumConc BC is defined as \f$ D\frac{du}{dn}+\frac{u}{2} = 0\f$, where u is neutron flux.
 * Hence, \f$ D\frac{du}{dn}=-\frac{u}{2} \f$ and \f$ -\frac{u}{2} \f$ is substituted into
 * the Neumann BC term produced from integrating the diffusion operator by parts.
 */
class VacuumConcBC : public IntegratedBC, public ScalarTransportBase
{
public:
  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  VacuumConcBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  /// Ratio of u to du/dn
  Real _alpha;
};

#endif // VACUUMCONCBC_H
