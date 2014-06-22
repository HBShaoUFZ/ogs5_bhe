/**
* \file BHE_2U.cpp
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with 2 U-tube
*
*/

#include "BHE_2U.h"
#include "makros.h"

using namespace BHE; 

/**
  * return the thermal resistance for the inlet pipline
  * idx is the index, when 2U case,
  * 0 - the first u-tube
  * 1 - the second u-tube
  */
double BHE_2U::get_thermal_resistance_fig(std::size_t idx = 0)
{
	// TODO
	return 0.0;
}

/**
  * return the thermal resistance for the outlet pipline
  * idx is the index, when 2U case,
  * 0 - the first u-tube
  * 1 - the second u-tube
  */
double BHE_2U::get_thermal_resistance_fog(std::size_t idx = 0)
{
	// TODO
	return 0.0;
}

/**
  * return the thermal resistance
  */
double BHE_2U::get_thermal_resistance(std::size_t idx = 0)
{
	// TODO
	return 0.0; 
}

/**
  * calculate thermal resistance
  */
void BHE_2U::calc_thermal_resistances()
{
	// thermal resistance due to advective flow of refrigerant in the pipes
	// Eq. 31 in Diersch_2011_CG
	_R_adv_i1 = 1.0 / (_Nu(0) * lambda_r * PI); 
	_R_adv_o1 = 1.0 / (_Nu(1) * lambda_r * PI);
	_R_adv_i2 = 1.0 / (_Nu(2) * lambda_r * PI);
	_R_adv_o2 = 1.0 / (_Nu(3) * lambda_r * PI);

	// thermal resistance due to thermal conductivity of the pip wall material
	// Eq. 36 in Diersch_2011_CG
	double _R_con_a;
	_R_con_a = std::log(r_outer / r_inner) / ( 2.0 * PI * lambda_p ); 

	// thermal resistance due to the grout transition
	double chi;
	double d0; // the average outer diameter of the pipes
	double s; // diagonal distances of pipes
	d0 = 2.0 * r_inner; 
	s = omega * std::sqrt(2); 
	chi = std::log(std::sqrt(D*D + 4 * d0*d0) / 2 / std::sqrt(2) / d0) / std::log(D / 2 / d0);
	_R_g = std::acosh( (D*D + d0*d0 - s*s) / (2*D*d0) ) / (2 * PI * lambda_g * lambda_g) * (3.098 - 4.432 * s / D + 2.364 * s * s / D / D);
	_R_con_b = chi * _R_g; 
	// Eq. 29 and 30
	_R_fig = _R_adv_i1 + _R_adv_i2 + _R_con_a_i1 + _R_con_a_i2 + _R_con_b;
	_R_fog = _R_adv_o1 + _R_adv_o2 + _R_con_a_o1 + _R_con_a_o2 + _R_con_b;

	// thermal resistance due to grout-soil exchange
	_R_gs = (1-chi)*_R_g; 	

	// thermal resistance due to inter-grout exchange
	double R_ar_1, R_ar_2; 
	R_ar_1 = std::acosh( (s*s - d0*d0) / d0 / d0 ) / (2.0 * PI * lambda_g ); 
	R_ar_2 = std::acosh((2.0*s*s - d0*d0) / d0 / d0) / (2.0 * PI * lambda_g );
	_R_gg_1 = 2.0 * _R_gs * ( R_ar_1 - 2.0 * chi * _R_g ) / ( 2.0 * _R_gs - R_ar_1 + 2.0 * chi * _R_g ) ;
	_R_gg_2 = 2.0 * _R_gs * (R_ar_2 - 2.0 * chi * _R_g) / (2.0 * _R_gs - R_ar_2 + 2.0 * chi * _R_g);

}

/**
  * Nusselt number calculation
  */
void BHE_2U::calc_Nu()
{
	// see Eq. 32 in Diersch_2011_CG

	double tmp_Nu = 0.0;
	double gamma, xi; 
	double d; 

	d = 2.0 * r_inner; 

	if ( _Re < 2300.0 )
	{
		tmp_Nu = 4.364; 
	}
	else if ( _Re >= 2300.0 && _Re < 10000.0 )
	{
		gamma = (_Re - 2300) / (10000 - 2300);

		tmp_Nu = (1.0 - gamma) * 4.364; 
		tmp_Nu += gamma * ( ( 0.0308 / 8.0 * 1.0e4 * _Pr ) / ( 1.0 + 12.7 * std::sqrt(0.0308/8.0) * ( std::pow(_Pr, 2.0/3.0) - 1.0 ) ) * ( 1.0 + std::pow( d / L, 2.0/3.0 ) ) ); 

	}
	else if ( _Re > 10000.0 )
	{
		xi = pow(1.8 * std::log10(_Re) - 1.5, -2.0); 
		tmp_Nu = (xi / 8.0 * _Re * _Pr) / (1.0 + 12.7 * std::sqrt(xi / 8.0) * (std::pow(_Pr, 2.0 / 3.0) - 1.0)) * (1.0 + std::pow(d / L, 2.0 / 3.0));
	}

	_Nu(0) = tmp_Nu; 
	_Nu(1) = tmp_Nu;
	_Nu(2) = tmp_Nu;
	_Nu(3) = tmp_Nu;
}

/**
  * Renolds number calculation
  */
void BHE_2U::calc_Re()
{
	double u_norm, d; 
	u_norm = _u.norm();
	d = 2.0 * r_inner; // inner diameter of the pipeline

	_Re = u_norm * d / (mu_r / rho_r) ; 
}

/**
  * Prandtl number calculation
  */
void BHE_2U::calc_Pr()
{
	_Pr = mu_r * heat_cap_r / lambda_r; 
}

/**
  * calculate heat transfer coefficient
  */
void BHE_2U::calc_heat_transfer_coefficients()
{
	_PHI_fig = 1.0 / _R_fig * 1.0 / S_i;
	_PHI_fog = 1.0 / _R_fog * 1.0 / S_o;
	_PHI_gg_1 = 1.0 / _R_gg_1 * 1.0 / S_g1;
	_PHI_gg_2 = 1.0 / _R_gg_2 * 1.0 / S_g2;
	_PHI_gs = 1.0 / _R_gs * 1.0 / S_gs;
}

/**
  * flow velocity inside the pipeline
  */
void BHE_2U::calc_u()
{
	double tmp_u; 

	// which discharge type it is? 
	if (_discharge_type == BHE::BHE_DISCHARGE_TYPE_PARALLEL)
	{
		tmp_u = Q_r / (2.0 * PI * r_inner * r_inner);
	}
	else  // serial discharge type
	{
		tmp_u = Q_r / (PI * r_inner * r_inner);
	}

	_u(0) = tmp_u; 
	_u(1) = tmp_u;
	_u(2) = tmp_u;
	_u(3) = tmp_u;
}

double BHE_2U::get_mass_coeff(std::size_t idx_unknown)
{
    double mass_coeff = 0.0;

    switch (idx_unknown)
    {
    case 0:  // i1
        mass_coeff = rho_r * heat_cap_r;
        break;
    case 1:  // i2
        mass_coeff = rho_r * heat_cap_r;
        break;
    case 2:  // o1
        mass_coeff = rho_r * heat_cap_r;
        break;
    case 3:  // o2
        mass_coeff = rho_r * heat_cap_r;
        break;
    case 4:  // g1
        mass_coeff = rho_g * heat_cap_g;
        break;
    case 5:  // g2
        mass_coeff = rho_g * heat_cap_g;
        break;
    case 6:  // g3
        mass_coeff = rho_g * heat_cap_g;
        break;
    case 7:  // g4
        mass_coeff = rho_g * heat_cap_g;
        break;
    default:
        break;
    }

    return mass_coeff;
}

double BHE_2U::get_laplace_coeff(std::size_t idx_unknown)
{
	// Here we calculates the laplace coefficients in the governing 
	// equations of BHE. These governing equations can be found in 
	// 1) Diersch (2013) FEFLOW book on page 952, M.120-122, or
	// 2) Diersch (2011) Comp & Geosci 37:1122-1135, Eq. 10-18. 
    double laplace_coeff(0.0);

	switch (idx_unknown)
	{
	case 0:
		// pipe i1, Eq. 18
		laplace_coeff = lambda_r + rho_r * heat_cap_r * _u.norm(); 
		break;
	case 1:
		// pipe o1, Eq. 18
		laplace_coeff = lambda_r + rho_r * heat_cap_r * _u.norm();
		break;
	case 2:
		// pipe i2,  Eq. 18
		laplace_coeff = lambda_r + rho_r * heat_cap_r * _u.norm();
		break;
	case 3:
		// pipe o2,  Eq. 18
		laplace_coeff = lambda_r + rho_r * heat_cap_r * _u.norm();
		break;
	case 4:
		// pipe g1, Eq. 14
		laplace_coeff = porosity_g * lambda_g; 
		break;
	case 5:
		// pipe g2, Eq. 15
		laplace_coeff = porosity_g * lambda_g;
		break;
	case 6:
		// pipe g3, Eq. 16
		laplace_coeff = porosity_g * lambda_g;
		break;
	case 7:
		// pipe g4, Eq. 17
		laplace_coeff = porosity_g * lambda_g;
		break;
	default:
		std::cout << "Error !!! The index passed to get_laplace_coeff for BHE is not correct. \n";
		exit(1);
		break;
	}
    return laplace_coeff;
}

double BHE_2U::get_advection_coeff(std::size_t idx_unknown)
{
    double advection_coeff(0);
    // TODO
    return advection_coeff;
}