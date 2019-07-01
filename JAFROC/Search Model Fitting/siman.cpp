#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <float.h>
#include <nr.h>
#include <gsl/gsl_siman.h>
#include <gsl/gsl_rng.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float LL_ROC_POI( float *parameters_fwd);
float LL_AFROC_POI( float *parameters_fwd);
float LL_FROC_POI( float *parameters_fwd);

extern int error_sm_fit ;
extern int fit_curve ;



/* set up parameters for this simulated annealing run */

/* how many points do we try before stepping */
#define N_TRIES 200    //200         

/* how many iterations for each T? */
#define ITERS_FIXED_T 200   // 10

/* max step size in random walk */
#define STEP_SIZE 0.0010		//0.001

/* Boltzmann constant */
#define K 1.0                   

/* initial temperature */
#define T_INITIAL 0.002         // 0.002

/* damping factor for temperature */
#define MU_T 1.005          // 1.005         
#define T_MIN 2.0e-10      //2.0e-15

gsl_siman_params_t params 
  = {N_TRIES, ITERS_FIXED_T, STEP_SIZE,
     K, T_INITIAL, MU_T, T_MIN};

gsl_siman_params_t params_hybrid 
  = {N_TRIES, ITERS_FIXED_T, 0.01,
     K, 0.01, MU_T, 2.0e-5};

/* now some functions to test in one dimension */

static double distance(double *a, double *b)
{
   int n;
   double dist;

   dist = 0;
   for (n=0; n<n_parameters_sm_fit; n++)
      if (fabs(a[n]-b[n]) > dist)
         dist = fabs(a[n]-b[n]);

   return (dist);
}



double L1(void *xp)
{
	float parameters[ 30 ], temp ;
//	int n_parameters ;
	int i ;
	double *x = (double *)xp ;

	if (error_sm_fit != 0) return 0;

	for( i = 0 ; i < n_parameters_sm_fit ; i++ ) parameters[ i ] = (float)x[ i ] ;
	
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



double M1(void *xp, void *yp)
{
	double *x = (double *)xp ;
	double *y = (double *)yp;

	double rms = 0 ;
	int i ;

	for( i = 0 ; i < n_parameters_sm_fit ; i++ ) {
		rms += ( x[ i ] - y[ i ] ) * ( x[ i ] - y[ i ] ) ;
	}
	rms = sqrt( rms ) ;

	return rms ;
}

void S1(const gsl_rng * r, void *xp, double step_size)
{
	int n ;
	double *old_x = (double *) xp;
	double *new_x;
	double u ; 

	new_x = (double *)malloc( n_parameters_sm_fit * sizeof( double ) ) ;

	for( n = 0 ; n < n_parameters_sm_fit ; n++ ) {
		u = gsl_rng_uniform(r);
		new_x[ n ] = u * 2 * step_size - step_size + old_x[ n ] ;
	}

	for( n = 0 ; n < n_parameters_sm_fit ; n++ ) {
		old_x[ n ] = new_x[ n ] ;
	}

	free( new_x ) ;
}

void P1(void *xp)
{
//  printf ("%12g", ((double *)xp)[0] );
}


void CT( void *source, void *dest, int n_parameters )
{

	memcpy( dest, source, n_parameters) ;

}



void DT( void *xp )
{

	free( xp ) ;

}


/////////// from gsl

void 
my_gsl_siman_solve (const gsl_rng * r, 
					double *x0_p, 
					gsl_siman_Efunc_t Ef,
					gsl_siman_step_t take_step2,
					gsl_siman_metric_t distance,
					gsl_siman_print_t print_position,
					gsl_siman_copy_t copyfunc,
					gsl_siman_copy_construct_t copy_constructor,
					gsl_siman_destroy_t destructor,
					size_t n_parameters,
					gsl_siman_params_t params)
{
	double x[MAX_PARAMETERS], new_x[MAX_PARAMETERS], best_x[MAX_PARAMETERS];
	double E, new_E, best_E;
	int i, i1, done;
	double T;
	int n_evals = 1, n_iter = 0, n_accepts, n_rejects, n_eless;

//	assert((copyfunc != NULL && copy_constructor != NULL && destructor != NULL)
//	|| (n_parameters != 0));

	E = Ef(x0_p);

	for (i1=0;i1<n_parameters;i1++) {
		x[i1] = x0_p[i1];
		best_x[i1] = x0_p[i1];
	}

//	LL_best = best_E = E;
	best_E = E;

	T = params.t_initial;
	done = 0;

	while (!done) {
		if (error_sm_fit != 0) return;

		n_accepts = 0;
		n_rejects = 0;
		n_eless = 0;
		for (i = 0; i < params.iters_fixed_T; ++i) {
			if (error_sm_fit != 0) return;

			for (i1=0;i1<n_parameters;i1++) new_x[i1] = x[i1];

			take_step2 (r, new_x, params.step_size);
			new_E = Ef (new_x);

			//if(new_E <= best_E) {
			//	for (i1=0;i1<n_parameters;i1++) best_x[i1] = new_x[i1];
			//	LL_best = best_E = new_E;
			//}

			++n_evals;
			/* now take the crucial step: see if the new point is accepted
			or not, as determined by the boltzman probability */
			if (new_E < E) {
			/* yay! take a step */
				for (i1=0;i1<n_parameters;i1++) {
					x[i1] = new_x[i1];
					best_x[i1] = new_x[i1];
				}
//				LL_best = best_E = new_E;
				best_E = new_E;
				E = new_E;
				++n_eless;
			} else if (gsl_rng_uniform(r) < exp (-(new_E - E)/(params.k * T)) ) {
				/* yay! take a step */
				for (i1=0;i1<n_parameters;i1++) x[i1] = new_x[i1];
				E = new_E;
				++n_accepts;
			} else ++n_rejects;
		}

		/* apply the cooling schedule to the temperature */
		/* FIXME: I should also introduce a cooling schedule for the iters */
		T /= params.mu_t;
		++n_iter;

		if (T < params.t_min) {
			done = 1;
		}

	}

	  /* at the end, copy the result onto the initial point, so we pass it
		 back to the caller */
  	  for (i1=0;i1<n_parameters;i1++) x0_p[i1] = best_x[i1];  
}



extern gsl_rng *r ;


void siman( float *parameters, int n_parms )
{
	int i ;
	double param[ MAX_PARAMETERS ] ;

	for( i = 0 ; i < n_parms ; i++ ) param[ i ] = (double)parameters[ i ] ;
/*
	if( fit_method == 2 ) {
		my_gsl_siman_solve(r, param, L1, S1, M1, P1,
			NULL, NULL, NULL,
			n_parameters, params_hybrid);
	} else {
		my_gsl_siman_solve(r, param, L1, S1, M1, P1,
			NULL, NULL, NULL,
			n_parameters, params);
	}
*/
	my_gsl_siman_solve(r, param, L1, S1, M1, P1,
		NULL, NULL, NULL,
		n_parms, params);

	for( i = 0 ; i < n_parms ; i++ ) parameters[ i ] = (float)param[ i ] ;
}
