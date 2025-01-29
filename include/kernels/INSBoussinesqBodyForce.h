#pragma once

#include "Kernel.h"

/**
 * Computes a body force that approximates natural buoyancy in
 * problems where there aren't very large variations in density.
 * See <a href="https://en.wikipedia.org/wiki/Boussinesq_approximation_(buoyancy)"> wikipedia </a>.
 */
class INSBoussinesqBodyForce : public Kernel
{
public:
  INSBoussinesqBodyForce(const InputParameters & parameters);

  static InputParameters validParams();

  virtual ~INSBoussinesqBodyForce() {}

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned jvar) override;

  // Parameters
  const VariableValue & _T;
  unsigned int _T_id;
  RealVectorValue _gravity;
  unsigned _component;

  // Material properties
  const MaterialProperty<Real> & _alpha;
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _T_ref;
};
