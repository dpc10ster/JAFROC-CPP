#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <nr.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern int total_F_calls ;
void ForwardTransform_all(float * parameters_in, float * parameters_fwd) ;
void InverseTransform_all(float * parameters_in, float * parameters_fwd) ;

float find_mu_minimize_G(float *parameters_fwd, int n_parameters) ;

//extern int MAX_CASES_LL_sm_fit[10];



float LL_ROC_POI( float *parameters_fwd)
{
	float parameters_nor[ MAX_PARAMETERS ];
	float mu, lambda, beta, nu, zeta[ MAX_PARAMETERS ], z ;
	float prob, rval;
	int i, SUM_FP, SUM_TP, L, FP_res, TP_res ;
	int R = R_sm_fit, n_parameters = n_parameters_sm_fit ;
	float lambda_k[ 20 ], temp1, temp2 ;
	int k ;
		
	if (error_sm_fit != 0) return 0;

	rval = 0;
	InverseTransform_all( parameters_fwd, parameters_nor) ;

	lambda = parameters_nor[ 0 ] ;
	beta = parameters_nor[ 1 ] ;
	mu = find_mu_minimize_G( parameters_fwd, n_parameters ) ;

	if (error_sm_fit != 0) return 0;

	nu = beta_to_nu( beta, mu );

	zeta[0] = -30;
	zeta[R+1] = +30;
	for (i = 0; i < R; i++) zeta[i+1] = parameters_nor [i + 2];
	
	// False Positives
	temp1 = 0;
	for ( i = 1; i < R ; i++) {
		prob = 
			exp(HALF*lambda*NR::erff(HALF_ROOT_TWO*zeta[i+1]))-exp(HALF*lambda*NR::erff(HALF_ROOT_TWO*zeta[i]));
		prob *= exp(-HALF*lambda);
		temp1 += prob;

		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += FP_sm_fit[i-1]*log(prob);
	}
	
	i = R;
	prob = 1 - exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*zeta[i]));
	temp1 += prob;
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += FP_sm_fit[i-1]*log(prob);

	SUM_FP = 0; for ( i = 0; i < R ;i++) SUM_FP += FP_sm_fit[i]; FP_res = MAX_CASES[ 0 ] - SUM_FP;

	i = 1;
	prob = exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*zeta[i]));
	temp1 += prob;
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += FP_res*log(prob);


	// True Positives
	temp2 = 0;
	for( L = 1 ; L <= MAX_LL ; L++ ) { // L = 
		for ( i = 1; i < R ; i++) {
			z = zeta[i+1];
			prob = 
				pow(1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu)),L)*exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z));

			z = zeta[i];
			prob -= 
				pow(1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu)),L)*exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z));
			temp2 += prob;

			//prob *= exp(-lambda);

			if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
			if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
			rval += TP_sm_fit[ L - 1 ][ i - 1 ]*log(prob);
		}
			
		z = zeta[R];
		prob = 
			pow(1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu)),L)*exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z));
		prob = 1 - prob;
		temp2 += prob;
		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += TP_sm_fit[ L - 1 ][ R - 1 ]*log(prob);
		
		SUM_TP = 0; for ( i = 0; i < R ;i++) SUM_TP += TP_sm_fit[ L - 1 ][ i ]; TP_res = MAX_CASES_LL_sm_fit[ L - 1 ] - SUM_TP;
		
		z = zeta[1];
		prob = 
			pow(1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu)),L)*exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z));
			
		temp2 += prob;
		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += TP_res*log(prob);

	}

	return -rval;

}



float LL_AFROC_POI( float *parameters_fwd )
{
	float parameters_nor[ MAX_PARAMETERS ];
	float mu, lambda, beta, nu, zeta[ MAX_PARAMETERS ], prob, rval ;
	int i, SUM_FP, SUM_LL, FP_res, LL_res ;
	int R = R_sm_fit, n_parameters = n_parameters_sm_fit ;

	if (error_sm_fit != 0) return 0;
	total_F_calls++ ;
	rval = 0;
	InverseTransform_all( parameters_fwd, parameters_nor) ;

	lambda = parameters_nor[ 0 ] ;
	beta = parameters_nor[ 1 ] ;
	mu = find_mu_minimize_G( parameters_fwd, n_parameters ) ;
	nu = beta_to_nu( beta, mu );
	if (error_sm_fit != 0) return 0;

	zeta[0] = -30;
	zeta[R+1] = +30;
	for (i = 0; i < R; i++) zeta[i+1] = parameters_nor [i + 2];
	
	// False Positives
	for ( i = 1; i < R ; i++) {
		prob = 
			(
				(
					(float)exp(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i+1])))
				)

			-	
				(
					(float)exp(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i])))
				)
			);

		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += FP_sm_fit[i-1]*(float)log((double)prob);
	}
	
	i = R;
	prob = 1 - exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*zeta[i]));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += FP_sm_fit[i-1]*log(prob);

	SUM_FP = 0; for ( i = 0; i < R ;i++) SUM_FP += FP_sm_fit[i]; FP_res = MAX_CASES[ 0 ] - SUM_FP;

	i = 1;
	prob = exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*zeta[i]));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += FP_res*log(prob);


	// Lesion Localization

	for ( i = 1; i < R ; i++) {
		prob = 
			(
				(
					1-HALF*nu*(1-NR::erff(HALF_ROOT_TWO*(zeta[i+1]-mu)))
					
				)

			-	
				(
					1-HALF*nu*(1-NR::erff(HALF_ROOT_TWO*(zeta[i]-mu)))
				)
			);

		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += LL_sm_fit[i-1]*(float)log((double)prob);
	}
	
	i = R;
	prob = HALF*nu*(1-NR::erff(HALF_ROOT_TWO*(zeta[i]-mu)));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += LL_sm_fit[i-1]*log(prob);

	SUM_LL = 0; for ( i = 0; i < R ;i++) SUM_LL += LL_sm_fit[i]; LL_res = Total_Lesions_sm_fit - SUM_LL;

	i = 1;
	prob = 1-HALF*nu*(1-NR::erff(HALF_ROOT_TWO*(zeta[i]-mu)));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += LL_res*log(prob);


	return -rval;

}


float pq(float * zeta,float lambda, float mu, int i)
{
	return HALF*(NR::erff(HALF_ROOT_TWO*(zeta[i+1]-mu)) - NR::erff(HALF_ROOT_TWO*(zeta[i]-mu)));
}


float LL_FROC_POI( float *parameters_fwd )
{
	float parameters_nor[ MAX_PARAMETERS ];
	float mu, lambda, beta, nu, zeta[ MAX_PARAMETERS ], prob, rval;
	int i, F, T, N_I, N_L;
	int R = R_sm_fit, n_parameters = n_parameters_sm_fit ;

	if (error_sm_fit != 0) return 0;

	N_I = (MAX_CASES[0]+MAX_CASES[1]);
	F = 0; for ( i = 0; i < R ; i++)  F += NL_sm_fit[i];
	T = 0; for ( i = 0; i < R ; i++)  T += LL_sm_fit[i];
	N_L = 0; for ( i = 0; i < MAX_CASES[1] ; i++)  N_L += N_Lesions_sm_fit[i];
	
	total_F_calls++ ;
	rval = 0;
	InverseTransform_all( parameters_fwd, parameters_nor) ;

	lambda = parameters_nor[ 0 ] ;
	beta = parameters_nor[ 1 ] ;
	mu = find_mu_minimize_G( parameters_fwd, n_parameters ) ;
	nu = beta_to_nu( beta, mu );

	if (error_sm_fit != 0) return 0;

	zeta[0] = -30;
	zeta[R+1] = +30;
	for (i = 0; i < R; i++) zeta[i+1] = parameters_nor [i + 2];

	
	// summation over i
	for ( i = 1; i <= R ; i++) {
		rval +=
		LL_sm_fit[i-1] * log(pq(zeta,lambda,mu,i));
		rval +=
		NL_sm_fit[i-1] * log(pq(zeta,lambda,0.0,i));
	}

	rval += F*log(lambda) - N_I*lambda + T*log(nu) + N_I*lambda*pq(zeta,lambda,0.0,0)
		+(N_L-T)*log(1-nu+nu*pq(zeta,lambda,mu,0));
 	
	return -rval;

}
