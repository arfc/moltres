#pragma once

#include "IntegratedBC.h"

/**
 * This kernel implements the Laplacian operator:
 * \f$\nabla u \cdot \nabla \phi_i\f$
 */
class DiffusionNoBCBC : public IntegratedBC
{
public:
  DiffusionNoBCBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;
};
