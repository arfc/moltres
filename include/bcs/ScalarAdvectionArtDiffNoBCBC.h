#ifndef SCALARADVECTIONARTDIFFNOBCBC_H
#define SCALARADVECTIONARTDIFFNOBCBC_H

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class ScalarAdvectionArtDiffNoBCBC;

template<>
InputParameters validParams<ScalarAdvectionArtDiffNoBCBC>();

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class ScalarAdvectionArtDiffNoBCBC : public ScalarTransportBase<IntegratedBC>
{
public:
  ScalarAdvectionArtDiffNoBCBC(const InputParameters & parameters);

  virtual ~ScalarAdvectionArtDiffNoBCBC(){}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  Real _scale;
  // Coupled variables
  const VariableValue & _u_vel;
  const VariableValue & _v_vel;
  const VariableValue & _w_vel;

  // Variable numberings
  unsigned _u_vel_var_number;
  unsigned _v_vel_var_number;
  unsigned _w_vel_var_number;

  VariableValue _u_def;
  VariableValue _v_def;
  VariableValue _w_def;
};


#endif // SCALARADVECTIONARTDIFFNOBCBC_H
