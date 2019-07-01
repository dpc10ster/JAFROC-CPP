#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <nr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define MAX_PARAMETERS 30

// temp hjy


float beta_to_nu( float beta, float mu ) ;
float pq(float * zeta,float lambda, float mu, int i) ;


/*
extern int MAX_CASES[2], LL[20], FP[20], *N_LESIONS_PER_IMAGE, TOTAL_LESIONS, n_parameters, R;
extern int error;
extern int TO_FIT_FLAG;
extern int MAX_L, MAX_CASES_LL[10];
*/
//extern int MAX_CASES_LL_sm_fit[ 10 ] ;
extern int fit_curve ;



float chisqr(int flag, float mu, float lambda, float nu, int R, float * zeta)
{
int i, SUM_FP, SUM_LL, FP_res, LL_res ;
float prob, rval;


	rval = 0;

	// False Positives
	for ( i = 1; i < R ; i++) {
		prob = 
			(
				(
					(float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i+1]))))
				)

			-	
				(
					(float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i]))))
				)
			);

		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += (float)pow( (double)( FP_sm_fit[i-1] - prob * (float)MAX_CASES[ 0 ] ), 2 ) / ( prob * (float)MAX_CASES[ 0 ] ) ;
	}
	
	i = R;
	prob = 1.0f - (float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i]))));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += (float)pow( (double)( FP_sm_fit[i-1] - prob * (float)MAX_CASES[ 0 ] ), 2 ) / ( prob * (float)MAX_CASES[ 0 ] ) ;

	SUM_FP = 0; for ( i = 0; i < R ;i++) SUM_FP += FP_sm_fit[i]; FP_res = MAX_CASES[ 0 ] - SUM_FP;

	i = 1;
	prob = (float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i]))));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	rval += (float)pow( (double)( FP_res - prob * (float)MAX_CASES[ 0 ] ), 2 ) / ( prob * (float)MAX_CASES[ 0 ] ) ;

	if ( flag == 1 ) {

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
			rval += (float)pow( (double)( LL_sm_fit[ i - 1 ] - prob * (float)Total_Lesions_sm_fit ), 2.0 ) / ( prob * (float)Total_Lesions_sm_fit ) ;
		}


		i = R;
		prob = HALF*nu*(1-NR::erff(HALF_ROOT_TWO*(zeta[i]-mu)));
		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += (float)pow( (double)( LL_sm_fit[ i - 1 ] - prob * (float)Total_Lesions_sm_fit ), 2.0 ) / ( prob * (float)Total_Lesions_sm_fit ) ;

		SUM_LL = 0; for ( i = 0; i < R ;i++) SUM_LL += LL_sm_fit[i]; LL_res = Total_Lesions_sm_fit - SUM_LL;

		i = 1;
		prob = 1-HALF*nu*(1-NR::erff(HALF_ROOT_TWO*(zeta[i]-mu)));
		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		rval += (float)pow( (double)( LL_res - prob * (float)Total_Lesions_sm_fit ), 2.0 ) / ( prob * (float)Total_Lesions_sm_fit ) ;
		
	}

	return rval;

}



float chisqr_ROC(float mu, float lambda, float nu, int R, float * zeta)
{
int i, SUM_FP, SUM_LL, FP_res, LL_res ;
float prob, rval, z ;
int k ;
float obs, expt ;


	rval = 0;

	// False Positives
	for ( i = 1; i < R ; i++) {
		prob = 
			(
				(
					(float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i+1]))))
				)

			-	
				(
					(float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i]))))
				)
			);

		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
		if( MAX_CASES[ 0 ] != 0 )
			rval += (float)pow( (double)( FP_sm_fit[i-1] - prob * (float)MAX_CASES[ 0 ] ), 2 ) / ( prob * (float)MAX_CASES[ 0 ] ) ;
	}
	
	i = R;
	prob = 1.0f - (float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i]))));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	if( MAX_CASES[ 0 ] != 0 )
		rval += (float)pow( (double)( FP_sm_fit[i-1] - prob * (float)MAX_CASES[ 0 ] ), 2 ) / ( prob * (float)MAX_CASES[ 0 ] ) ;

	SUM_FP = 0; for ( i = 0; i < R ;i++) SUM_FP += FP_sm_fit[i]; FP_res = MAX_CASES[ 0 ] - SUM_FP;

	i = 1;
	prob = (float)exp((double)(-HALF*lambda+HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*zeta[i]))));
	if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
	if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
	if( MAX_CASES[ 0 ] != 0 )
		rval += (float)pow( (double)( FP_res - prob * (float)MAX_CASES[ 0 ] ), 2 ) / ( prob * (float)MAX_CASES[ 0 ] ) ;

	// True Positives
//	for( int L = 1 ; L <= MAX_LL ; L++ ) { // L = 
	for ( i = 1; i < R ; i++) {
		obs = 0.0f ;
		expt = 0.0f ;
		for( int L = 1 ; L <= MAX_LL ; L++ ) {
			z = zeta[i+1];
			prob = 
				exp(-lambda)*
				(-(1-pow((1-nu+nu*(HALF*NR::erff(HALF_ROOT_TWO*(z-mu))+HALF)),L))
				+1+pow((1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu))),L)
				*(exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z))*exp(lambda)-1));

			z = zeta[i];
			prob -= 
				exp(-lambda)*
				(-(1-pow((1-nu+nu*(HALF*NR::erff(HALF_ROOT_TWO*(z-mu))+HALF)),L))
				+1+pow((1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu))),L)
				*(exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z))*exp(lambda)-1));

			if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
			if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
//			if( MAX_CASES_LL_sm_fit[ L - 1 ] != 0 )
//				rval += (float)pow( (double)( TP_sm_fit[ L - 1 ][ i - 1 ] - prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ), 2 ) / ( prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ) ;
			expt += prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ;
			obs += TP_sm_fit[ L - 1 ][ i - 1 ] ;
		}
		if( expt != 0.0f )
			rval += (float)pow( (double)( obs - expt ), 2 ) / ( expt ) ;
	}
	
	obs = 0.0f ;
	expt = 0.0f ;

	for( int L = 1 ; L <= MAX_LL ; L++ ) {
		z = zeta[R];
		prob = 
			exp(-lambda)*
			(-(1-pow((1-nu+nu*(HALF*NR::erff(HALF_ROOT_TWO*(z-mu))+HALF)),L))
			+1+pow((1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu))),L)
			*(exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z))*exp(lambda)-1));
		prob = 1 - prob;
		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
//		if( MAX_CASES_LL_sm_fit[ L - 1 ] != 0 )
//			rval += (float)pow( (double)( TP_sm_fit[ L - 1 ][ R - 1 ] - prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ), 2 ) / ( prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ) ;
		expt += prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ;
		obs += TP_sm_fit[ L - 1 ][ R - 1 ] ;
	}
	if( expt != 0.0f )
		rval += (float)pow( (double)( obs - expt ), 2 ) / ( expt ) ;
		
	obs = 0.0f ;
	expt = 0.0f ;
	for( int L = 1 ; L <= MAX_LL ; L++ ) {		
		int SUM_TP = 0; for ( i = 0; i < R ;i++) SUM_TP += TP_sm_fit[ L - 1 ][ i ] ; int TP_res = MAX_CASES_LL_sm_fit[ L - 1 ] - SUM_TP;
		
		z = zeta[1];
		prob = 
			exp(-lambda)*
			(-(1-pow((1-nu+nu*(HALF*NR::erff(HALF_ROOT_TWO*(z-mu))+HALF)),L))
			+1+pow((1-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu))),L)
			*(exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z))*exp(lambda)-1));
			
		if (fabs(prob-1) < (float)1e-15) prob = 1-(float)1e-15;
		if (fabs(prob) < (float)1e-15) prob = (float)1e-15;
//		if( MAX_CASES_LL_sm_fit[ L - 1 ] != 0 )
//			rval += (float)pow( (double)( TP_res - prob * (float)MAX_CASES_LL_sm_fit[ L-1 ] ), 2 ) / ( prob * (float)MAX_CASES_LL_sm_fit[ L - 1 ] ) ;
		expt += prob * (float)MAX_CASES_LL_sm_fit[ L-1 ] ;
		obs += TP_res ;
	}
	if( expt != 0.0f )
		rval += (float)pow( (double)( obs - expt ), 2 ) / ( expt ) ;

	return rval;
}


float chisqr_FROC(int flag, float mu, float lambda, float nu, int R, float * zeta)
{
int i, N_I, N_L;
float p_i, q_i, rval;


	N_I = (MAX_CASES[0]+MAX_CASES[1]);
	N_L = 0; for ( i = 0; i < MAX_CASES[1] ; i++)  N_L += N_Lesions_Per_Image[i];

	rval = 0;

	for ( i = 1; i <= R ; i++) {
		p_i = pq(zeta,lambda,0.0,i);
		if (fabs(p_i-1) < (float)1e-15) p_i = 1-(float)1e-15;
		if (fabs(p_i) < (float)1e-15) p_i = (float)1e-15;
		rval += pow((NL_sm_fit[i-1] - p_i*lambda*N_I ), 2) / (p_i*lambda*N_I) ;
	}
	

	if ( flag == 1 ) {
		for ( i = 1; i <= R ; i++) {
			q_i = pq(zeta,lambda,mu,i);
			if (fabs(q_i-1) < (float)1e-15) q_i = 1-(float)1e-15;
			if (fabs(q_i) < (float)1e-15) q_i = (float)1e-15;
			rval += pow((LL_sm_fit[i-1] - q_i*nu*N_L ), 2) / (q_i*nu*N_L) ;
		}		
	}

	return rval;

}




