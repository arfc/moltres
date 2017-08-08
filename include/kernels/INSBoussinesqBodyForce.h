#ifndef INSBOUSSINESQBODYFORCE_H
#define INSBOUSSINESQBODYFORCE_H

#include "Kernel.h"

// Forward Declarations
class INSBoussinesqBodyForce;

template <>
InputParameters validParams<INSBoussinesqBodyForce>();

/**
 * Computes a body force that approximates natural buoyancy in
 * problems where there aren't very large variations in density.
 * See <a href="https://en.wikipedia.org/wiki/Boussinesq_approximation_(buoyancy)"> wikipedia </a>.
 */
class INSBoussinesqBodyForce : public Kernel
{
public:
  INSBoussinesqBodyForce(const InputParameters & parameters);

  virtual ~INSBoussinesqBodyForce() {}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  // Parameters
  const VariableValue & _dT;
  unsigned int _dT_id;
  RealVectorValue _gravity;
  unsigned _component;

  // Material properties
  const MaterialProperty<Real> & _alpha;
  const MaterialProperty<Real> & _rho;
};

#endif
