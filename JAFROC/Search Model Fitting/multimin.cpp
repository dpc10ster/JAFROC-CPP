#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <float.h>
#include <nr.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_rng.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float LL_ROC_POI( float *parameters_fwd);
float LL_AFROC_POI( float *parameters_fwd);
float LL_FROC_POI( float *parameters_fwd);

extern int error_sm_fit ;
//extern int TO_FIT_FLAG ;
extern int fit_curve ;

// hjy - prevent oscillatory property
float save_lambda, save_beta, save_ll ;
int converged ;



double my_LL_func( const gsl_vector *v, void *params )
{
float parameters[ 30 ], temp ;
int n_parameters ;
int i ;

	if (error_sm_fit != 0) return 0;

	n_parameters = v->size ;
	for( i = 0 ; i < n_parameters ; i++ ) parameters[ i ] = (float)gsl_vector_get( v, i ) ;
	
	switch( fit_curve ) {
		case FIT_CURVE_ROC:
			temp = LL_ROC_POI( parameters);
			break ;
		case FIT_CURVE_AFROC:
			temp = LL_AFROC_POI( parameters);
			break ;
		case FIT_CURVE_FROC:
			temp = LL_FROC_POI( parameters);
			break ;
	}
	
	if (error_sm_fit != 0) return 0;

	return (double) temp;
}




void multimin( float *parameters, int n_parameters)
{
	float lowest = (float) 1e5;
	int count = 0;
	const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex ;

	converged = 0 ;
	save_ll = 1e15f ;

	gsl_multimin_fminimizer *s = NULL ;
	gsl_vector *ss, *x ;
	gsl_multimin_function minex_func ;

	size_t iter = 0 ;
	int status, i ;
	double size ;
	float prec; prec = (float) 1e-5;
	
	x = gsl_vector_alloc( n_parameters ) ;
	for( i = 0 ; i < n_parameters ; i++ ) gsl_vector_set( x, i, parameters[ i ] ) ;

	/* Set initial step sizes to 1 */
	ss = gsl_vector_alloc( n_parameters ) ;
	gsl_vector_set_all( ss, 1.0 ) ;

	/* Initialize method and iterate */
	minex_func.n = n_parameters ;
	minex_func.f = &my_LL_func ;
	minex_func.params = NULL ;

	s = gsl_multimin_fminimizer_alloc( T, n_parameters ) ;
	gsl_multimin_fminimizer_set( s, &minex_func, x, ss ) ;

	do {
		iter++ ;
		status = gsl_multimin_fminimizer_iterate( s ) ;
	   
		if( error_sm_fit != 0 ) {
			gsl_vector_free( x ) ;
			gsl_vector_free( ss ) ;
			gsl_multimin_fminimizer_free( s ) ;
			return ;
		}
		if( status ) break;

		size = gsl_multimin_fminimizer_size( s ) ;
		status = gsl_multimin_test_size( size, prec ) ;
		if ((s->fval - lowest) < 0.0001) lowest = (float)s->fval; else count += 1;
		if (count > 100) break;

	 }
	while( status == GSL_CONTINUE && iter < 100000 && converged == 0 ) ;

	for( i = 0 ; i < n_parameters ; i++ ) parameters[ i ] = (float)gsl_vector_get( s->x, i ) ;

	gsl_vector_free( x ) ;
	gsl_vector_free( ss ) ;
	gsl_multimin_fminimizer_free( s ) ;
}
