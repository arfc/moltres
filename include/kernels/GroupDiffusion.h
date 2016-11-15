#ifndef GROUPDIFFUSION_H
#define GROUPDIFFUSION_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declaration
class GroupDiffusion;

template<>
InputParameters validParams<GroupDiffusion>();


class GroupDiffusion : public Kernel,
                       public ScalarTransportBase
{
public:
  GroupDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<std::vector<Real> > & _diffcoef;
  const MaterialProperty<std::vector<Real> > & _d_diffcoef_d_temp;
  int _group;
  unsigned int _temp_id;
};

#endif //GROUPDIFFUSION_H
