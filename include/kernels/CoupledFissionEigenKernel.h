#ifndef COUPLEDFISSIONEIGENKERNEL_H
#define COUPLEDFISSIONEIGENKERNEL_H

#include "EigenKernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class CoupledFissionEigenKernel;

template <>
InputParameters validParams<CoupledFissionEigenKernel>();

/**
 * Computes the fission source normalized by eigenvalue. In other words:
 * \f[
 *   \frac{1}{k} \sum_g \nu \Sigma_{g,f} \phi_g
 * \f]
 */
class CoupledFissionEigenKernel : public EigenKernel, public ScalarTransportBase
{
public:
  CoupledFissionEigenKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _chi_t;
  const MaterialProperty<std::vector<Real>> & _chi_p;
  const MaterialProperty<Real> & _beta;
  unsigned int _group;
  unsigned int _num_groups;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  bool _account_delayed;
};

#endif // COUPLEDFISSIONEIGENKERNEL_H
