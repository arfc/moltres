#ifndef SIGMAR_H
#define SIGMAR_H

#include "Kernel.h"
#include "MaterialProperty.h"

// Forward Declaration
class SigmaR;

template<>
InputParameters validParams<SigmaR>();


class SigmaR : public Kernel
{
public:
  SigmaR(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<std::vector<Real> > & _remxs;
  const MaterialProperty<std::vector<Real> > & _d_remxs_d_temp;
  int _group;
  unsigned int _temp_id;
};

#endif //SIGMAR_H
