#ifndef MATINSTEMPERATURENOBCBC_H
#define MATINSTEMPERATURENOBCBC_H

#include "IntegratedBC.h"

// Forward Declarations
class MatINSTemperatureNoBCBC;

template<>
InputParameters validParams<MatINSTemperatureNoBCBC>();

/**
 * This class implements the "No BC" boundary condition
 * discussed by Griffiths, Papanastiou, and others.
 */
class MatINSTemperatureNoBCBC : public IntegratedBC
{
public:
  MatINSTemperatureNoBCBC(const InputParameters & parameters);

  virtual ~MatINSTemperatureNoBCBC(){}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  const MaterialProperty<Real> & _k;
};


#endif // MATINSTEMPERATURENOBCBC_H
