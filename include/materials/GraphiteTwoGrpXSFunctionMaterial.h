#ifndef GRAPHITETWOGRPXSFUNCTIONMATERIAL_H_
#define GRAPHITETWOGRPXSFUNCTIONMATERIAL_H_

#include "GenericConstantMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"
#include "MonotoneCubicInterpolation.h"
#include "LinearInterpolation.h"

class GraphiteTwoGrpXSFunctionMaterial;

template <>
InputParameters validParams<GraphiteTwoGrpXSFunctionMaterial>();

class GraphiteTwoGrpXSFunctionMaterial : public GenericConstantMaterial
{
public:
  GraphiteTwoGrpXSFunctionMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  const VariableValue & _T;
  // const MaterialProperty<Real> & _rho;

  MaterialProperty<std::vector<Real>> & _remxs;
  MaterialProperty<std::vector<Real>> & _fissxs;
  MaterialProperty<std::vector<Real>> & _nsf;
  MaterialProperty<std::vector<Real>> & _fisse;
  MaterialProperty<std::vector<Real>> & _diffcoef;
  MaterialProperty<std::vector<Real>> & _recipvel;
  MaterialProperty<std::vector<Real>> & _chi;
  MaterialProperty<std::vector<Real>> & _gtransfxs;
  MaterialProperty<std::vector<Real>> & _beta_eff;
  MaterialProperty<Real> & _beta;
  MaterialProperty<std::vector<Real>> & _decay_constant;
  MaterialProperty<std::vector<Real>> & _d_remxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;
  MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;
  MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;
  MaterialProperty<std::vector<Real>> & _d_chi_d_temp;
  MaterialProperty<std::vector<Real>> & _d_gtransfxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_beta_eff_d_temp;
  MaterialProperty<Real> & _d_beta_d_temp;
  MaterialProperty<std::vector<Real>> & _d_decay_constant_d_temp;
};

#endif // GRAPHITETWOGRPXSFUNCTIONMATERIAL_H
