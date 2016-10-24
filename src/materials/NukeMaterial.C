#include "NukeMaterial.h"
#include "MooseUtils.h"

template<>
InputParameters validParams<Gas>()
{
	InputParameters params = validParams<Material>();
	params.addRequiredParam<FileName>("property_tables_file", "The file containing interpolation tables for material properties.");
	return params;
}


Gas::Gas(const InputParameters & parameters) :
	Material(parameters),
{
  std::vector<Real> flux;
  std::vector<Real> remxs;
  std::vector<Real> fissxs;
  std::vector<Real> nubar;
  std::vector<Real> nsf;
  std::vector<Real> fisse;
  std::vector<Real> diffcoef;
  std::vector<Real> chi;
  std::vector<Real> gransfxs;

	std::string file_name = getParam<FileName>("property_tables_file");
	MooseUtils::checkFileReadable(file_name);
	const char *charPath = file_name.c_str();
	std::ifstream myfile (charPath);
	Real value;

	if (myfile.is_open())
	{
	while ( myfile >> value )
	{
	actual_mean_energy.push_back(value);
	myfile >> value;
	alpha.push_back(value);
	myfile >> value;
	alphaEx.push_back(value);
	myfile >> value;
	alphaEl.push_back(value);
	myfile >> value;
	mu.push_back(value);
	myfile >> value;
	diff.push_back(value);
	}
	myfile.close();
	}

	else std::cerr << "Unable to open file" << std::endl;

	_alpha_interpolation.setData(actual_mean_energy, alpha);
	_alphaEx_interpolation.setData(actual_mean_energy, alphaEx);
	_alphaEl_interpolation.setData(actual_mean_energy, alphaEl);
	_mu_interpolation.setData(actual_mean_energy, mu);
	_diff_interpolation.setData(actual_mean_energy, diff);
}

void
Gas::computeQpProperties()
{
	_massem[_qp] = 9.11e-31;
	_massGas[_qp] = 40.0*1.66e-27;
	_massArp[_qp] = 40.0*1.66e-27;
	_T_gas[_qp] = _user_T_gas;
	_p_gas[_qp] = _user_p_gas;
	_k_boltz[_qp] = 1.38e-23;
	if (_use_moles)
	_n_gas[_qp] = _p_gas[_qp] / (8.3145 * _T_gas[_qp]);
	else
	_n_gas[_qp] = _p_gas[_qp] / (_k_boltz[_qp] * _T_gas[_qp]);

	_se_coeff[_qp] = _user_se_coeff;
	_work_function[_qp] = _user_work_function;
	_field_enhancement[_qp] = _user_field_enhancement;

	_Richardson_coefficient[_qp] = _user_Richardson_coefficient;
	_cathode_temperature[_qp] = _user_cathode_temperature;

	_se_energy[_qp] = 2. * 3. / 2.; // Emi uses 2 Volts coming off the wall (presumably for Te). Multiply by 3/2 to get mean_en
	_e[_qp] = 1.6e-19;
	_eps[_qp] = 8.85e-12;
	_sgnem[_qp] = -1.;
	_sgnmean_en[_qp] = -1.;
	_sgnArp[_qp] = 1.;
	_diffpotential[_qp] = _eps[_qp];

	_TArp[_qp] = 300;

	// With the exception of temperature/energy (generally in eV), all properties are in standard SI units unless otherwise indicated

	// if (!_jac_test)
	// {
	if (_interp_trans_coeffs)
	{
	if (_ramp_trans_coeffs)
	{
	_muem[_qp] = (std::tanh(_t / 1e-6) * _mu_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) + (1. - std::tanh(_t / 1e-6)) * .0352) * _voltage_scaling;
	_d_muem_d_actual_mean_en[_qp] = std::tanh(_t / 1e-6) * _mu_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
	_diffem[_qp] = std::tanh(_t / 1e-6) * _diff_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) + (1. - std::tanh(_t / 1e-6)) * .30;
	_d_diffem_d_actual_mean_en[_qp] = std::tanh(_t / 1e-6) * _diff_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	}
	else
	{
	_muem[_qp] = _mu_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
	_d_muem_d_actual_mean_en[_qp] = _mu_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
	_diffem[_qp] = _diff_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	_d_diffem_d_actual_mean_en[_qp] = _diff_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	}
	}
	else
	{
	// From bolos at atmospheric pressure and an EField of 2e5 V/m
	_muem[_qp] = 0.0352103411399 * _voltage_scaling; // units of m^2/(kV*s) if _voltage_scaling = 1000
	_d_muem_d_actual_mean_en[_qp] = 0.0;
	_diffem[_qp] = 0.297951680159;
	_d_diffem_d_actual_mean_en[_qp] = 0.0;
	}

	// From Richards and Sawin, muArp*pressure = 1444 cm^2*Torr/(V*s) and diffArp*pressure = 40 cm^2*Torr/s. Use pressure = 760 torr.
	_muArp[_qp] = 1444. * _voltage_scaling / (10000. * 760. * _p_gas[_qp] / 1.01E5); // units of m^2/(kV*s) if _voltage_scaling = 1000
	_diffArp[_qp] = .004 / (760. * _p_gas[_qp] / 1.01E5); //covert to m^2 and include press

	// 100 times less than electrons
	// _muArp[_qp] = 3.52e-4;
	// _diffArp[_qp] = 2.98e-3;

	// From curve fitting with bolos
	_iz_coeff_efield_a[_qp] = 1.43171672e-1;
	_iz_coeff_efield_b[_qp] = 9.05925536e-1;
	_iz_coeff_efield_c[_qp] = 3.04958892e+6;

	// if (_townsend) {
	_iz_coeff_energy_a[_qp] = 1.52165930e+8;
	_iz_coeff_energy_b[_qp] = -2.87277596e-1;
	_iz_coeff_energy_c[_qp] = 5.51972192e+1;
	// }
	// else {
	//	 _iz_coeff_energy_a[_qp] = 1.43878529e-11;
	//	 _iz_coeff_energy_b[_qp] = -2.70610234e-1;
	//	 _iz_coeff_energy_c[_qp] = 7.64727794e+1;
	// }

	_actual_mean_energy[_qp] = std::exp(_mean_en[_qp] - _em[_qp]);
	_alpha_iz[_qp] = _alpha_interpolation.sample(_actual_mean_energy[_qp]);
	_d_iz_d_actual_mean_en[_qp] = _alpha_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	// _d_iz_d_actual_mean_en[_qp] = _d_alpha_d_actual_mean_energy_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	_alpha_ex[_qp] = _alphaEx_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	_d_ex_d_actual_mean_en[_qp] = _alphaEx_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	if (_interp_elastic_coeff) {
	_alpha_el[_qp] = _alphaEl_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	_d_el_d_actual_mean_en[_qp] = _alphaEl_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	}
	else {
	_alpha_el[_qp] = 5e8;
	_d_el_d_actual_mean_en[_qp] = 0.0;
	}

	_el_coeff_energy_a[_qp] = 1.60638169e-13;
	_el_coeff_energy_b[_qp] = 3.17917979e-1;
	_el_coeff_energy_c[_qp] = 4.66301096;

	_N_A[_qp] = 6.02e23;
	_Ar[_qp] = 1.01e5/(300*1.38e-23);
	_Eiz[_qp] = 15.76;
	_Eex[_qp] = 11.5;

	// From Hagelaar: The below approximations can be derived assumption Maxwell EEDF, const momentum-transfer frequency, and constant kinetic pressure.
	_mumean_en[_qp] = 5.0/3.0*_muem[_qp];
	_diffmean_en[_qp] = 5.0/3.0*_diffem[_qp];
	if (_interp_trans_coeffs) {
	_d_mumean_en_d_actual_mean_en[_qp] = 5.0/3.0*_d_muem_d_actual_mean_en[_qp];
	_d_diffmean_en_d_actual_mean_en[_qp] = 5.0/3.0*_d_diffem_d_actual_mean_en[_qp];
	}
	else {
	_d_mumean_en_d_actual_mean_en[_qp] = 0.0;
	_d_diffmean_en_d_actual_mean_en[_qp] = 0.0;
	}

	_rate_coeff_elastic[_qp] = 1e-13;

	_TemVolts[_qp] = 2. / 3. * std::exp(_mean_en[_qp] - _em[_qp]);
	_Tem[_qp] = _e[_qp] * _TemVolts[_qp] / _k_boltz[_qp];

	_kiz[_qp] = 2.34e-14 * std::pow(_TemVolts[_qp], .59) * std::exp(-17.44 / _TemVolts[_qp]);
	_d_kiz_d_actual_mean_en[_qp] = 2.34e-14 * (.59 * std::pow(_TemVolts[_qp], .59 - 1.) * std::exp(-17.44 / _TemVolts[_qp]) + std::pow(_TemVolts[_qp], .59) * std::exp(-17.44 / _TemVolts[_qp]) * 17.44 / std::pow(_TemVolts[_qp], 2.)) * 2. / 3.;
	_kex[_qp] = 2.48e-14 * std::pow(_TemVolts[_qp], .33) * std::exp(-12.78 / _TemVolts[_qp]);
	_d_kex_d_actual_mean_en[_qp] = 2.48e-14 * (.33 * std::pow(_TemVolts[_qp], .33 - 1.) * std::exp(-12.78 / _TemVolts[_qp]) + std::pow(_TemVolts[_qp], .33) * std::exp(-12.78 / _TemVolts[_qp]) * 12.78 / std::pow(_TemVolts[_qp], 2.)) * 2. / 3.;
	// _kel[_qp] = 2.3363-14 * std::pow(_TemVolts[_qp], 1.609) * std::exp(.0618 * std::pow(std::log(_TemVolts[_qp]), 2.) - .1171 * std::pow(std::log(_TemVolts[_qp]), 3.));
	// _d_kel_d_actual_mean_en[_qp] = 2.3363e-14 * (1.609 * std::pow(_TemVolts[_qp], 1.609 - 1.) * std::exp(.0618 * std::pow(std::log(_TemVolts[_qp]), 2.) - .1171 * std::pow(std::log(_TemVolts[_qp]), 3.)) + std::pow(_TemVolts[_qp], 1.609) * std::exp(.0618 * std::pow(std::log(_TemVolts[_qp]), 2.) - .1171 * std::pow(std::log(_TemVolts[_qp]), 3.)) * (.0618 * 2. * std::log(_TemVolts[_qp]) / _TemVolts[_qp] - .1171 * 3. * std::pow(std::log(_TemVolts[_qp]), 2.) / _TemVolts[_qp])) * 2. / 3.;
	_kel[_qp] = 1e-13; // Approximate elastic rate coefficient
	_d_kel_d_actual_mean_en[_qp] = 0.;
	if (_use_moles)
	{
	_kiz[_qp] = _kiz[_qp] * _N_A[_qp];
	_d_kiz_d_actual_mean_en[_qp] = _d_kiz_d_actual_mean_en[_qp] * _N_A[_qp];
	_kex[_qp] = _kex[_qp] * _N_A[_qp];
	_d_kex_d_actual_mean_en[_qp] = _d_kex_d_actual_mean_en[_qp] * _N_A[_qp];
	_kel[_qp] = _kel[_qp] * _N_A[_qp];
	_d_kel_d_actual_mean_en[_qp] = _d_kel_d_actual_mean_en[_qp] * _N_A[_qp];
	}
	// }

	// // This block is for Jacobian testing

	// else
	// {
	//	 if (_interp_trans_coeffs)
	//	 {
	//	 if (_ramp_trans_coeffs)
	//	 {
	//	 _muem[_qp] = (std::tanh(_t / 1e-6) * _mu_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) + (1. - std::tanh(_t / 1e-6)) * .0352) * _voltage_scaling;
	//	 _d_muem_d_actual_mean_en[_qp] = std::tanh(_t / 1e-6) * _mu_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
	//	 _diffem[_qp] = std::tanh(_t / 1e-6) * _diff_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) + (1. - std::tanh(_t / 1e-6)) * .30;
	//	 _d_diffem_d_actual_mean_en[_qp] = std::tanh(_t / 1e-6) * _diff_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 }
	//	 else
	//	 {
	//	 _muem[_qp] = _mu_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
	//	 _d_muem_d_actual_mean_en[_qp] = _mu_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
	//	 _diffem[_qp] = _diff_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 _d_diffem_d_actual_mean_en[_qp] = _diff_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 }
	//	 }
	//	 else
	//	 {
	//	 // From bolos at atmospheric pressure and an EField of 2e5 V/m
	//	 _muem[_qp] = 0.0352103411399 * _voltage_scaling; // units of m^2/(kV*s) if _voltage_scaling = 1000
	//	 _d_muem_d_actual_mean_en[_qp] = 0.0;
	//	 _diffem[_qp] = 0.297951680159;
	//	 _d_diffem_d_actual_mean_en[_qp] = 0.0;
	//	 }

	//	 // From Richards and Sawin, muArp*pressure = 1444 cm^2*Torr/(V*s) and diffArp*pressure = 40 cm^2*Torr/s. Use pressure = 760 torr.
	//	 _muArp[_qp] = 1.9e-4 * _voltage_scaling; // units of m^2/(kV*s) if _voltage_scaling = 1000
	//	 _diffArp[_qp] = 5.26e-6;

	//	 // 100 times less than electrons
	//	 // _muArp[_qp] = 3.52e-4;
	//	 // _diffArp[_qp] = 2.98e-3;

	//	 // From curve fitting with bolos
	//	 _iz_coeff_efield_a[_qp] = 1.43171672e-1;
	//	 _iz_coeff_efield_b[_qp] = 9.05925536e-1;
	//	 _iz_coeff_efield_c[_qp] = 3.04958892e+6;

	//	 // if (_townsend) {
	//	 _iz_coeff_energy_a[_qp] = 1.52165930e+8;
	//	 _iz_coeff_energy_b[_qp] = -2.87277596e-1;
	//	 _iz_coeff_energy_c[_qp] = 5.51972192e+1;
	//	 // }
	//	 // else {
	//	 //	 _iz_coeff_energy_a[_qp] = 1.43878529e-11;
	//	 //	 _iz_coeff_energy_b[_qp] = -2.70610234e-1;
	//	 //	 _iz_coeff_energy_c[_qp] = 7.64727794e+1;
	//	 // }

	//	 _actual_mean_energy[_qp] = std::exp(_mean_en[_qp] - _em[_qp]);
	//	 _alpha_iz[_qp] = _alpha_interpolation.sample(_actual_mean_energy[_qp]);
	//	 _d_iz_d_actual_mean_en[_qp] = _alpha_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 // _d_iz_d_actual_mean_en[_qp] = _d_alpha_d_actual_mean_energy_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 _alpha_ex[_qp] = _alphaEx_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 _d_ex_d_actual_mean_en[_qp] = _alphaEx_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 if (_interp_elastic_coeff) {
	//	 _alpha_el[_qp] = _alphaEl_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 _d_el_d_actual_mean_en[_qp] = _alphaEl_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
	//	 }
	//	 else {
	//	 _alpha_el[_qp] = 5e8;
	//	 _d_el_d_actual_mean_en[_qp] = 0.0;
	//	 }

	//	 _el_coeff_energy_a[_qp] = 1.60638169e-13;
	//	 _el_coeff_energy_b[_qp] = 3.17917979e-1;
	//	 _el_coeff_energy_c[_qp] = 4.66301096;

	//	 _N_A[_qp] = 6.02e23;
	//	 _Ar[_qp] = 1.01e5/(300*1.38e-23);
	//	 _Eiz[_qp] = 15.76;
	//	 _Eex[_qp] = 11.5;

	//	 // From Hagelaar: The below approximations can be derived assumption Maxwell EEDF, const momentum-transfer frequency, and constant kinetic pressure.
	//	 _mumean_en[_qp] = 5.0/3.0*_muem[_qp];
	//	 _diffmean_en[_qp] = 5.0/3.0*_diffem[_qp];
	//	 if (_interp_trans_coeffs) {
	//	 _d_mumean_en_d_actual_mean_en[_qp] = 5.0/3.0*_d_muem_d_actual_mean_en[_qp];
	//	 _d_diffmean_en_d_actual_mean_en[_qp] = 5.0/3.0*_d_diffem_d_actual_mean_en[_qp];
	//	 }
	//	 else {
	//	 _d_mumean_en_d_actual_mean_en[_qp] = 0.0;
	//	 _d_diffmean_en_d_actual_mean_en[_qp] = 0.0;
	//	 }

	//	 _rate_coeff_elastic[_qp] = 1e-13;
	//	 _massem[_qp] = 9.11e-31;
	//	 _massGas[_qp] = 40.0*1.66e-27;
	//	 _massArp[_qp] = 40.0*1.66e-27;
	//	 _T_gas[_qp] = _user_T_gas;
	//	 _p_gas[_qp] = _user_p_gas;
	//	 _k_boltz[_qp] = 1.38e-23;
	//	 if (_use_moles)
	//	 _n_gas[_qp] = _p_gas[_qp] / (8.3145 * _T_gas[_qp]);
	//	 else
	//	 _n_gas[_qp] = _p_gas[_qp] / (_k_boltz[_qp] * _T_gas[_qp]);

	//	 _se_coeff[_qp] = _user_se_coeff;
	//	 _se_energy[_qp] = 2. * 3. / 2.; // Emi uses 2 Volts coming off the wall (presumably for Te). Multiply by 3/2 to get mean_en
	//	 _e[_qp] = 1.6e-19;
	//	 _eps[_qp] = 8.85e-12;
	//	 _sgnem[_qp] = -1.;
	//	 _sgnmean_en[_qp] = -1.;
	//	 _sgnArp[_qp] = 1.;
	//	 _diffpotential[_qp] = _eps[_qp];

	//	 _TArp[_qp] = 300;

	//	 _TemVolts[_qp] = 2. / 3. * std::exp(_mean_en[_qp] - _em[_qp]);
	//	 _Tem[_qp] = _e[_qp] * _TemVolts[_qp] / _k_boltz[_qp];

	//	 _kiz[_qp] = 2.34e-14 * std::pow(_TemVolts[_qp], .59) * std::exp(-17.44 / _TemVolts[_qp]);
	//	 _d_kiz_d_actual_mean_en[_qp] = 2.34e-14 * (.59 * std::pow(_TemVolts[_qp], .59 - 1.) * std::exp(-17.44 / _TemVolts[_qp]) + std::pow(_TemVolts[_qp], .59) * std::exp(-17.44 / _TemVolts[_qp]) * 17.44 / std::pow(_TemVolts[_qp], 2.)) * 2. / 3.;
	//	 _kex[_qp] = 2.48e-14 * std::pow(_TemVolts[_qp], .33) * std::exp(-12.78 / _TemVolts[_qp]);
	//	 _d_kex_d_actual_mean_en[_qp] = 2.48e-14 * (.33 * std::pow(_TemVolts[_qp], .33 - 1.) * std::exp(-12.78 / _TemVolts[_qp]) * std::pow(_TemVolts[_qp], .33) * std::exp(-12.78 / _TemVolts[_qp]) * 12.78 / std::pow(_TemVolts[_qp], 2.)) * 2. / 3.;
	//	 _kel[_qp] = 2.3363-14 * std::pow(_TemVolts[_qp], 1.609) * std::exp(.0618 * std::pow(std::log(_TemVolts[_qp]), 2.) - .1171 * std::pow(std::log(_TemVolts[_qp]), 3.));
	//	 _d_kel_d_actual_mean_en[_qp] = 2.3363e-14 * (1.609 * std::pow(_TemVolts[_qp], 1.609 - 1.) * std::exp(.0618 * std::pow(std::log(_TemVolts[_qp]), 2.) - .1171 * std::pow(std::log(_TemVolts[_qp]), 3.)) + std::pow(_TemVolts[_qp], 1.609) * std::exp(.0618 * std::pow(std::log(_TemVolts[_qp]), 2.) - .1171 * std::pow(std::log(_TemVolts[_qp]), 3.)) * (.0618 * 2. * std::log(_TemVolts[_qp]) / _TemVolts[_qp] - .1171 * 3. * std::pow(std::log(_TemVolts[_qp]), 2.) / _TemVolts[_qp])) * 2. / 3.;
	//	 if (_use_moles)
	//	 {
	//	 _kiz[_qp] = _kiz[_qp] * _N_A[_qp];
	//	 _d_kiz_d_actual_mean_en[_qp] = _d_kiz_d_actual_mean_en[_qp] * _N_A[_qp];
	//	 _kex[_qp] = _kex[_qp] * _N_A[_qp];
	//	 _d_kex_d_actual_mean_en[_qp] = _d_kex_d_actual_mean_en[_qp] * _N_A[_qp];
	//	 _kel[_qp] = _kel[_qp] * _N_A[_qp];
	//	 _d_kel_d_actual_mean_en[_qp] = _d_kel_d_actual_mean_en[_qp] * _N_A[_qp];
	//	 }
	// }
}
