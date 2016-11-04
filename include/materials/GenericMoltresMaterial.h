#ifndef GENERICMOLTRESMATERIAL_H_
#define GENERICMOLTRESMATERIAL_H_

#include "GenericConstantMaterial.h"
#include "SplineInterpolation.h"

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
  MaterialProperty<std::vector<Real> > & _d_remxs_d_temp;
  MaterialProperty<std::vector<Real> > & _d_fissxs_d_temp;
  MaterialProperty<std::vector<Real> > & _d_nsf_d_temp;
  MaterialProperty<std::vector<Real> > & _d_fisse_d_temp;
  MaterialProperty<std::vector<Real> > & _d_diffcoef_d_temp;
  MaterialProperty<std::vector<Real> > & _d_recipvel_d_temp;
  MaterialProperty<std::vector<Real> > & _d_chi_d_temp;
  MaterialProperty<std::vector<Real> > & _d_gtransfxs_d_temp;

  unsigned int _num_groups;
  std::map<std::string, std::vector<SplineInterpolation> > _xsec_interpolators;
  std::map<std::string, int> _vec_lengths;
};

#endif //GENERICMOLTRESMATERIAL_H
