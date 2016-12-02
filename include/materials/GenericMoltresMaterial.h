#ifndef GENERICMOLTRESMATERIAL_H_
#define GENERICMOLTRESMATERIAL_H_

#include "GenericConstantMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class GenericMoltresMaterial;

template<>
InputParameters validParams<GenericMoltresMaterial>();

class GenericMoltresMaterial : public GenericConstantMaterial
{
public:
  GenericMoltresMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  const VariableValue & _temperature;

  MaterialProperty<std::vector<Real> > & _remxs;
  MaterialProperty<std::vector<Real> > & _fissxs;
  MaterialProperty<std::vector<Real> > & _nsf;
  MaterialProperty<std::vector<Real> > & _fisse;
  MaterialProperty<std::vector<Real> > & _diffcoef;
  MaterialProperty<std::vector<Real> > & _recipvel;
  MaterialProperty<std::vector<Real> > & _chi;
  MaterialProperty<std::vector<Real> > & _gtransfxs;
  MaterialProperty<std::vector<Real> > & _beta_eff;
  MaterialProperty<std::vector<Real> > & _decay_constant;
  MaterialProperty<std::vector<Real> > & _d_remxs_d_temp;
  MaterialProperty<std::vector<Real> > & _d_fissxs_d_temp;
  MaterialProperty<std::vector<Real> > & _d_nsf_d_temp;
  MaterialProperty<std::vector<Real> > & _d_fisse_d_temp;
  MaterialProperty<std::vector<Real> > & _d_diffcoef_d_temp;
  MaterialProperty<std::vector<Real> > & _d_recipvel_d_temp;
  MaterialProperty<std::vector<Real> > & _d_chi_d_temp;
  MaterialProperty<std::vector<Real> > & _d_gtransfxs_d_temp;
  MaterialProperty<std::vector<Real> > & _d_beta_eff_d_temp;
  MaterialProperty<std::vector<Real> > & _d_decay_constant_d_temp;

  bool _bivariable_interp;
  const PostprocessorValue & _other_temp;
  bool _monotonic_interpolation;

  int _num_groups;
  int _num_precursor_groups;
  std::map<std::string, std::vector<SplineInterpolation> > _xsec_spline_interpolators;
  std::map<std::string, std::vector<BicubicSplineInterpolation> > _xsec_bicubic_spline_interpolators;
  std::map<std::string, int> _vec_lengths;
  std::string _material;

  std::vector<std::vector<Real> > _flux_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _remxs_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _fissxs_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _nubar_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _nsf_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _fisse_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _diffcoeff_consts = std::vector<std::vector<Real> >(2);
  std::vector<std::vector<Real> > _recipvel_consts = std::vector<std::vector<Real> >(2);
};

#endif //GENERICMOLTRESMATERIAL_H
