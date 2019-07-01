#include "stdafx.h"
#include "../common.h"
#include <gsl/gsl_rng.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int FP_sm_fit[ 20 ], NL_sm_fit[ 20 ], LL_sm_fit[ 20 ], **TP_sm_fit ;
int *N_Lesions_sm_fit = NULL, Total_Lesions_sm_fit, n_parameters_sm_fit, R_sm_fit ;
int MAX_L_sm_fit, *MAX_CASES_LL_sm_fit = NULL ;
int error_sm_fit = 0 ;

gsl_rng *r = NULL ;
int total_F_calls = 0 ;

void ForwardTransform_all(float * parameters_in, float * parameters_fwd) ;
void InverseTransform_all(float * parameters_in, float * parameters_fwd) ;

float LL_ROC_POI( float *parameters_fwd ) ;
float LL_AFROC_POI( float *parameters_fwd ) ;
float LL_FROC_POI( float *parameters_fwd ) ;
float find_mu_minimize_G(float *parameters_fwd, int n_parameters) ;

float chisqr_mu ;
int fit_curve ;


void get_max_cases_ll( void )
{
	if( MAX_CASES_LL_sm_fit != NULL ) delete[] MAX_CASES_LL_sm_fit ;
	MAX_CASES_LL_sm_fit = new int[ MAX_LL ] ;
	for( int i = 0 ; i < MAX_LL ; i++ ) {
		int count = 0 ;
		for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
			if( N_Lesions_Per_Image[ k ] == i + 1 ) count++ ;
		}
		MAX_CASES_LL_sm_fit[ i ] = count ;
	}
}



bool constrained_fit_roc( int *fp_bin, int **tp_bin, float *mu1, float *chisqr, float *parameters, int n_parameters, int fit_method )
{
	const gsl_rng_type * T = NULL ;
	float mu ;
	float parameters_nor[MAX_PARAMETERS] ;
	float parameters_fwd[MAX_PARAMETERS] ;

	if( r == NULL ) {
		gsl_rng_env_setup();
		T = gsl_rng_default ;
		r = gsl_rng_alloc( T ) ;
	}

	get_max_cases_ll() ;

	fit_curve = FIT_CURVE_ROC ;
	error_sm_fit = 0 ;

	if( N_Lesions_sm_fit != NULL ) delete[] N_Lesions_sm_fit ;
	N_Lesions_sm_fit = new int[ MAX_CASES[ 1 ] ] ;
	for( int i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) N_Lesions_sm_fit[ i ] = N_Lesions_Per_Image[ i ] ;
	Total_Lesions_sm_fit = 0 ;
	for( int i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) Total_Lesions_sm_fit += N_Lesions_sm_fit[ i ] ;

	n_parameters_sm_fit = n_parameters ;
	R_sm_fit = n_parameters - 2 ;

	TP_sm_fit = new int *[ MAX_LL ] ;
	for( int l = 0 ; l < MAX_LL ; l++ ) {
		TP_sm_fit[ l ] = new int[ MAX_CTFFS ] ;
		for( int i = 0 ; i < R_sm_fit ; i++ ) {
			TP_sm_fit[ l ][ i ] = tp_bin[ l ][ i ] ;
		}
	}
	for( int i = 0 ; i < R_sm_fit ; i++ ) FP_sm_fit[ i ] = fp_bin[ i ] ;

	for( int i = 0 ; i < n_parameters_sm_fit ; i++ ) parameters_nor[ i ] = parameters[ i ] ;

	ForwardTransform_all (parameters_nor, parameters_fwd) ;
	float initial_ll = (float)LL_ROC_POI( parameters_fwd ) ;	

	if( fit_method == FIT_METHOD_SIMPLEX ) {
		multimin( parameters_fwd, n_parameters) ;
	} else if( fit_method == FIT_METHOD_SIMAN ) {
		siman( parameters_fwd, n_parameters ) ;
	} else if( fit_method == FIT_METHOD_HYBRID ) {
		siman( parameters_fwd, n_parameters ) ;
		multimin( parameters_fwd, n_parameters) ;
	} else if( fit_method == FIT_METHOD_AMEBSA ) {
		run_amebsa( parameters_fwd, n_parameters ) ;
	} else if( fit_method == FIT_METHOD_AMOEBA ) {
		run_amoeba( parameters_fwd, n_parameters ) ;
	}

	float final_ll = (float)LL_ROC_POI( parameters_fwd ) ;	
	mu = find_mu_minimize_G( parameters_fwd, n_parameters ) ;
	*mu1 = mu ;
	*chisqr = chisqr_mu ;

	delete[] N_Lesions_sm_fit ;
	N_Lesions_sm_fit = NULL ;

	for( int l = 0 ; l < MAX_LL ; l++ ) delete[] TP_sm_fit[ l ] ;
	delete[] TP_sm_fit ;

	if( error_sm_fit == 0 ) {
		InverseTransform_all( parameters_fwd, parameters_nor ) ;
		for( int i = 0 ; i < n_parameters ; i++ ) parameters[ i ] = parameters_nor[ i ] ;
		return true ;
	}

	return false ;
}


bool constrained_fit_afroc( int *fp_bin, int *ll_bin, float *mu1, float *chisqr, float *parameters, int n_parameters, int fit_method )
{
	const gsl_rng_type * T = NULL ;
	float mu ;
	float parameters_nor[MAX_PARAMETERS] ;
	float parameters_fwd[MAX_PARAMETERS] ;

	if( r == NULL ) {
		gsl_rng_env_setup();
		T = gsl_rng_default ;
		r = gsl_rng_alloc( T ) ;
	}

	fit_curve = FIT_CURVE_AFROC ;
	error_sm_fit = 0 ;

	if( N_Lesions_sm_fit != NULL ) delete[] N_Lesions_sm_fit ;
	N_Lesions_sm_fit = new int[ MAX_CASES[ 1 ] ] ;
	for( int i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) N_Lesions_sm_fit[ i ] = N_Lesions_Per_Image[ i ] ;
	Total_Lesions_sm_fit = 0 ;
	for( int i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) Total_Lesions_sm_fit += N_Lesions_sm_fit[ i ] ;

	n_parameters_sm_fit = n_parameters ;
	R_sm_fit = n_parameters - 2 ;

	for( int i = 0 ; i < R_sm_fit ; i++ ) LL_sm_fit[ i ] = ll_bin[ i ] ;
	for( int i = 0 ; i < R_sm_fit ; i++ ) FP_sm_fit[ i ] = fp_bin[ i ] ;

	for( int i = 0 ; i < n_parameters_sm_fit ; i++ ) parameters_nor[ i ] = parameters[ i ] ;

	ForwardTransform_all (parameters_nor, parameters_fwd) ;
	float initial_ll = (float)LL_AFROC_POI( parameters_fwd ) ;	

	if( fit_method == FIT_METHOD_SIMPLEX ) {
		multimin( parameters_fwd, n_parameters) ;
	} else if( fit_method == FIT_METHOD_SIMAN ) {
		siman( parameters_fwd, n_parameters ) ;
	} else if( fit_method == FIT_METHOD_HYBRID ) {
		siman( parameters_fwd, n_parameters ) ;
		multimin( parameters_fwd, n_parameters) ;
	} else if( fit_method == FIT_METHOD_AMEBSA ) {
		run_amebsa( parameters_fwd, n_parameters ) ;
	} else if( fit_method == FIT_METHOD_AMOEBA ) {
		run_amoeba( parameters_fwd, n_parameters ) ;
	}

	float final_ll = (float)LL_AFROC_POI( parameters_fwd ) ;
	mu = find_mu_minimize_G( parameters_fwd, n_parameters ) ;
	*mu1 = mu ;
	*chisqr = chisqr_mu ;

	delete[] N_Lesions_sm_fit ;
	N_Lesions_sm_fit = NULL ;

	if( error_sm_fit == 0 ) {
		InverseTransform_all( parameters_fwd, parameters_nor ) ;
		for( int i = 0 ; i < n_parameters ; i++ ) parameters[ i ] = parameters_nor[ i ] ;
		return true ;
	}

	return false ;
}


bool constrained_fit_froc( int *nl_bin, int *ll_bin, float *mu1, float *chisqr, float *parameters, int n_parameters, int fit_method )
{
	const gsl_rng_type * T = NULL ;
	float mu ;
	float parameters_nor[MAX_PARAMETERS] ;
	float parameters_fwd[MAX_PARAMETERS] ;

	if( r == NULL ) {
		gsl_rng_env_setup();
		T = gsl_rng_default ;
		r = gsl_rng_alloc( T ) ;
	}

	fit_curve = FIT_CURVE_FROC ;
	error_sm_fit = 0 ;

	if( N_Lesions_sm_fit != NULL ) delete[] N_Lesions_sm_fit ;
	N_Lesions_sm_fit = new int[ MAX_CASES[ 1 ] ] ;
	for( int i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) N_Lesions_sm_fit[ i ] = N_Lesions_Per_Image[ i ] ;
	Total_Lesions_sm_fit = 0 ;
	for( int i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) Total_Lesions_sm_fit += N_Lesions_sm_fit[ i ] ;

	n_parameters_sm_fit = n_parameters ;
	R_sm_fit = n_parameters - 2 ;

	for( int i = 0 ; i < R_sm_fit ; i++ ) LL_sm_fit[ i ] = ll_bin[ i ] ;
	for( int i = 0 ; i < R_sm_fit ; i++ ) NL_sm_fit[ i ] = nl_bin[ i ] ;

	for( int i = 0 ; i < n_parameters_sm_fit ; i++ ) parameters_nor[ i ] = parameters[ i ] ;

	ForwardTransform_all (parameters_nor, parameters_fwd) ;
	float initial_ll = (float)LL_FROC_POI( parameters_fwd ) ;	

	if( fit_method == FIT_METHOD_SIMPLEX ) {
		multimin( parameters_fwd, n_parameters) ;
	} else if( fit_method == FIT_METHOD_SIMAN ) {
		siman( parameters_fwd, n_parameters ) ;
	} else if( fit_method == FIT_METHOD_HYBRID ) {
		siman( parameters_fwd, n_parameters ) ;
		multimin( parameters_fwd, n_parameters) ;
	} else if( fit_method == FIT_METHOD_AMEBSA ) {
		run_amebsa( parameters_fwd, n_parameters ) ;
	} else if( fit_method == FIT_METHOD_AMOEBA ) {
		run_amoeba( parameters_fwd, n_parameters ) ;
	}

	float final_ll = (float)LL_FROC_POI( parameters_fwd ) ;
	mu = find_mu_minimize_G( parameters_fwd, n_parameters ) ;
	*mu1 = mu ;
	*chisqr = chisqr_mu ;

	delete[] N_Lesions_sm_fit ;
	N_Lesions_sm_fit = NULL ;

	if( error_sm_fit == 0 ) {
		InverseTransform_all( parameters_fwd, parameters_nor ) ;
		for( int i = 0 ; i < n_parameters ; i++ ) parameters[ i ] = parameters_nor[ i ] ;
		return true ;
	}

	return false ;
}