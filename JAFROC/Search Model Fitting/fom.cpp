#include "stdafx.h"
#include "../common.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <float.h>
#include <nr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float jafroc_mu, jafroc_lambda, jafroc_nu ;

int error;


float beta_to_nu( float beta, float mu )
{
	float nu = 1.0f - (float)exp( (double)-beta * fabs( (double)mu ) ) ;
	if (fabs(nu-1) < (float)1e-7) nu = (float)0.999;
	if (fabs(nu-0) < (float)1e-7) nu = (float)0.001;
	return nu ;
}



DP int_jafroc_fom( DP z )
{
	float mu = jafroc_mu ;
	float lambda = jafroc_lambda ;
	float nu = jafroc_nu ;

	return (float)( (HALF_ROOT_TWO*nu*(HALF-HALF*NR::erff(HALF_ROOT_TWO*(z-mu)))*lambda
		*exp(-HALF*pow(z,2.0)-HALF*lambda+HALF*lambda*
		NR::erff(HALF_ROOT_TWO*z))/pow(M_PI,(HALF))) ) ;
}



float jafroc_fom( float mu, float lambda, float nu )
{
	float result ;

	jafroc_mu = mu ;
	jafroc_lambda = lambda ;
	jafroc_nu = nu ;

	result = NR::qromb( int_jafroc_fom, -10.0, 10.0 ) ;
	
	result += nu * (float)exp( (double)-lambda ) + 0.5f * ( 1.0f - nu ) * (float)exp( (double)-lambda ) ;

	return result ;
}



float sm_mu, sm_lambda, sm_nu, sm_s ;

DP INT_SM_AUC_FOM_CONSTANT_LESION( DP z )
{
	float mu = sm_mu ;
	float lambda = sm_lambda ;
	float nu = sm_nu ;
	float s = sm_s ;

	float a = (float)exp((double)(-HALF*lambda + HALF*lambda*NR::erff((double)(HALF_ROOT_TWO*z)))) ;
	float b = (float)NR::erff((double)(HALF_ROOT_TWO*(z-mu))) ;
	float c = (float)pow((double)(1.0-nu+HALF*nu*b+HALF*nu),s) ;

	float rval = -(1.0f-c*a)*lambda*(float)exp((double)(-HALF*pow((double)z,2.0)))*a/(float)sqrt(M_PI*2.0) ;

	int ev = _fpclass( rval ) ;
	int i ;
	switch( ev ) {
	case _FPCLASS_SNAN:
		i = 0 ;
		break ;
	case _FPCLASS_QNAN:
		i = 0 ;
		break; 
	case _FPCLASS_NINF:
		i = 0 ;
		break ;
	case _FPCLASS_PINF:
		i = 0 ;
		break ;
	}

	return rval ;
}


float sm_auc_ind( float mu, float lambda, float nu, float s )
{
	float result ;

	sm_mu = mu ;
	sm_lambda = lambda ;
	sm_nu = nu ;
	sm_s = s ;

	result = NR::qromb( INT_SM_AUC_FOM_CONSTANT_LESION, 20.0, -20.0 ) ;
	result = result + (1.0f-(float)exp((double)-sm_lambda)*(float)pow((double)(1.0-sm_nu),(int)s))*
		(float)exp((double)-sm_lambda)+0.5f*(float)pow((exp((double)-sm_lambda)),2)*(float)pow((1.0-(double)sm_nu),(int)sm_s) ;

	return result ;
}


float sm_auc( float mu, float lambda, float nu, int *N_LESIONS_PER_IMAGE, int NCASES_ABN )
{
	int i ;
	float fom = 0.0, temp;
	int max_n_les = 0 ;
	int *n_cases_arr ;

	for( i = 0 ; i < NCASES_ABN ; i++ ) if( N_LESIONS_PER_IMAGE[ i ] > max_n_les ) max_n_les = N_LESIONS_PER_IMAGE[ i ] ;

	n_cases_arr = (int *)malloc( sizeof( int ) * ( max_n_les + 1 ) ) ;
	for( i = 1 ; i <=  max_n_les ; i++ ) n_cases_arr[ i ] = 0 ;

	for( i = 0 ; i < NCASES_ABN ; i++ ) n_cases_arr[ N_LESIONS_PER_IMAGE[ i ] ]++ ;
	
	for( i = 1 ; i <= max_n_les ; i++ ) {
		if( n_cases_arr[ i ] != 0 ) {
			temp = sm_auc_ind( mu, lambda, nu, (float)i ) * (float)n_cases_arr[ i ] / (float)NCASES_ABN ;
			fom += temp ;
		}
	}

	free( n_cases_arr ) ;

	return fom ;
}



float sm_auc_gamma( float mu, float lambda, float nu, float gamma, int *N_LESIONS_PER_IMAGE, int NCASES_ABN )
{
	int i ;
	float fom = 0.0, temp;
	int max_n_les = 0 ;
	int *n_cases_arr ;

	for( i = 0 ; i < NCASES_ABN ; i++ ) if( N_LESIONS_PER_IMAGE[ i ] > max_n_les ) max_n_les = N_LESIONS_PER_IMAGE[ i ] ;

	float *lambda_k = new float[ max_n_les + 1 ] ;
	for( int k = 0 ; k <= max_n_les ; k++ ) {
		lambda_k[ k ] = lambda * exp( -gamma * nu * (float)k ) ;	
	}

	n_cases_arr = (int *)malloc( sizeof( int ) * ( max_n_les + 1 ) ) ;
	for( i = 1 ; i <=  max_n_les ; i++ ) n_cases_arr[ i ] = 0 ;

	for( i = 0 ; i < NCASES_ABN ; i++ ) n_cases_arr[ N_LESIONS_PER_IMAGE[ i ] ]++ ;
	
	for( i = 1 ; i <= max_n_les ; i++ ) {
		if( n_cases_arr[ i ] != 0 ) {
			temp = sm_auc_ind( mu, lambda_k[ i ], nu, (float)i ) * (float)n_cases_arr[ i ] / (float)NCASES_ABN ;
			fom += temp ;
		}
	}

	free( n_cases_arr ) ;
	free( lambda_k ) ;

	return fom ;
}



float jafroc_fom_gamma( float mu, float lambda, float nu, float gamma, int *N_LESIONS_PER_IMAGE, int NCASES_ABN )
{
	int i ;
	float fom = 0.0, temp;
	int max_n_les = 0 ;
	int *n_cases_arr ;

	for( i = 0 ; i < NCASES_ABN ; i++ ) if( N_LESIONS_PER_IMAGE[ i ] > max_n_les ) max_n_les = N_LESIONS_PER_IMAGE[ i ] ;

	float *lambda_k = new float[ max_n_les + 1 ] ;
	for( int k = 0 ; k <= max_n_les ; k++ ) {
		lambda_k[ k ] = lambda * exp( -gamma * nu * (float)k ) ;	
	}

	n_cases_arr = (int *)malloc( sizeof( int ) * ( max_n_les + 1 ) ) ;
	for( i = 1 ; i <=  max_n_les ; i++ ) n_cases_arr[ i ] = 0 ;

	for( i = 0 ; i < NCASES_ABN ; i++ ) n_cases_arr[ N_LESIONS_PER_IMAGE[ i ] ]++ ;
	
	for( i = 1 ; i <= max_n_les ; i++ ) {
		if( n_cases_arr[ i ] != 0 ) {
			temp = jafroc_fom( mu, lambda_k[ i ], nu ) * (float)n_cases_arr[ i ] / (float)NCASES_ABN ;
			fom += temp ;
		}
	}

	free( n_cases_arr ) ;
	free( lambda_k ) ;

	return fom ;
}
