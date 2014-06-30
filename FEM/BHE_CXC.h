/**
* \file BHE_CXC.h
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with centred coaxial pipe
*
*/

#ifndef BHE_CXC_H
#define BHE_CXC_H

#include "BHEAbstract.h"
#include "makros.h"

namespace BHE  // namespace of borehole heat exchanger
{

	class BHE_CXC : public BHEAbstract
	{
	public:
		/**
		* constructor
		*/
        BHE_CXC(const std::string name             /* name of the BHE */,
                double my_L = 100                  /* length/depth of the BHE */,
	    		double my_D = 0.013                /* diameter of the BHE */,
		    	double my_Qr = 21.86 / 86400       /* total refrigerant flow discharge of BHE */,
			    double my_r_inner = 0.024          /* radius of the inner pipline */,
			    double my_r_outer = 0.05           /* radius of the outer pipline */,
			    double my_b_in = 0.003             /* pipe-in wall thickness*/,
			    double my_b_out = 0.004            /* pipe-out wall thickness*/,
			    double my_mu_r = 0.00054741        /* dynamic viscosity of the refrigerant */,
			    double my_rho_r = 988.1            /* density of the refrigerant */,
				double my_alpha_L = 1.0e-4         /* longitudinal dispersivity of the refrigerant in the pipeline */,
                double my_heat_cap_r = 4180        /* specific heat capacity of the refrigerant */,
                double my_rho_g = 2190             /* density of the grout */,
				double my_porosity_g = 0.5         /* porosity of the grout */,
                double my_heat_cap_g = 1000        /* specific heat capacity of the grout */,
			    double my_lambda_r = 0.6405        /* thermal conductivity of the refrigerant */,
			    double my_lambda_p = 0.38          /* thermal conductivity of the pipe wall */,
			    double my_lambda_g = 2.3           /* thermal conductivity of the grout */)
			: BHEAbstract(BHE::BHE_TYPE_CXA, name)
		{
			_u = Eigen::Vector2d::Zero();
			_Nu = Eigen::Vector2d::Zero();

			L = my_L;
			D = my_D;
			Q_r = my_Qr;
			r_inner = my_r_inner;
			r_outer = my_r_outer;
			b_in = my_b_in;
			b_out = my_b_out;
			mu_r = my_mu_r;
			rho_r = my_rho_r;
			alpha_L = my_alpha_L;
			heat_cap_r = my_heat_cap_r;
            rho_g = my_rho_g; 
            heat_cap_g = my_heat_cap_g; 
			porosity_g = my_porosity_g;
			lambda_r = my_lambda_r;
			lambda_p = my_lambda_p;
			lambda_g = my_lambda_g;

			// Table 1 in Diersch_2011_CG
			S_o = PI * 2.0 * r_outer;
			S_io = PI * 2.0 * r_inner;
			S_gs = PI * D;

			// initialization calculation
			initialize();
		};

		/**
		* return the number of unknowns needed for CXA BHE
		*/
		std::size_t get_n_unknowns() { return 3; }

        /**
        * return the number of boundary heat exchange terms for this BHE
        * abstract function, need to be realized.
        */
        std::size_t get_n_heat_exchange_terms() { return 3; }

		/**
		* return the thermal resistance for the inlet pipline
		* idx is the index, when 2U case,
		* 0 - the first u-tube
		* 1 - the second u-tube
		*/
		double get_thermal_resistance_fig(std::size_t idx);

		/**
		* return the thermal resistance for the outlet pipline
		* idx is the index, when 2U case,
		* 0 - the first u-tube
		* 1 - the second u-tube
		*/
		double get_thermal_resistance_fog(std::size_t idx);

		/**
		* return the thermal resistance
		*/
		double get_thermal_resistance(std::size_t idx);

		/**
		* calculate thermal resistance
		*/
		void calc_thermal_resistances();

		/**
		* Nusselt number calculation
		*/
		void calc_Nu();

		/**
		* Renolds number calculation
		*/
		void calc_Re();

		/**
		* Prandtl number calculation
		*/
		void calc_Pr();

		/**
		* flow velocity inside the pipeline
		*/
		void calc_u();

		/**
		* calculate heat transfer coefficient
		*/
		void calc_heat_transfer_coefficients();

        /**
          * return the coeff of mass matrix,
          * depending on the index of unknown.
          */
        double get_mass_coeff(std::size_t idx_unknown); 

        /**
          * return the coeff of laplace matrix,
          * depending on the index of unknown.
          */
        double get_laplace_coeff(std::size_t idx_unknown);

        /**
          * return the coeff of advection matrix,
          * depending on the index of unknown.
          */
        double get_advection_coeff(std::size_t idx_unknown);

        /**
          * return the coeff of boundary heat exchange matrix,
          * depending on the index of unknown.
          */
        double get_boundary_heat_exchange_coeff(std::size_t idx_unknown);

        /**
          * return the shift index based on primary variable value
          */
        int get_loc_shift_by_pv(FiniteElement::PrimaryVariable pv_name);

        /**
        * required by eigen library,
        * to make sure the dynamically allocated class has
        * aligned "operator new"
        */
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:
		/**
		* thermal resistances
		*/
		double _R_ff, _R_fog;

		/**
		* thermal resistances due to advective flow of refrigerant in the pipes
		*/
		double _R_adv_i1, _R_adv_a_o1, _R_adv_b_o1;

		/**
		* thermal resistances due to the pipe wall material
		*/
		double _R_con_i1, _R_con_o1;

		/**
		* thermal resistances due to the grout transition
		*/
		double _R_con_b;

		/**
		* thermal resistances of the grout
		*/
		double _R_g;

		/**
		* thermal resistances of the grout soil exchange
		*/
		double _R_gs;

		/**
		* heat transfer coefficients
		*/
		double _PHI_fog, _PHI_ff, _PHI_gs;

		/**
		* Reynolds number
		*/
		double _Re_o1, _Re_i1;

		/**
		* Prandtl number
		*/
		double _Pr;

		/**
		* Nusselt number
		*/
		Eigen::Vector2d _Nu;

		/**
		* flow velocity inside the pipeline
		*/
		Eigen::Vector2d _u;

		/**
		* specific exchange surfaces S
		*/
		double S_o, S_io, S_gs;

	};


}  // end of namespace

#endif