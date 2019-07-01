#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <float.h>
#include <nr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern float lambda_min, lambda_max, beta_min, beta_max, mu_min, mu_max; 
float brent_all[ MAX_PARAMETERS ];

void ForwardTransform_all(float * parameters_in, float * parameters_fwd) ;
void InverseTransform_all(float * parameters_in, float * parameters_fwd) ;
void ForwardTransform_mu(float * parameters_nor, float * parameters_fwd) ;
void InverseTransform_mu(float * parameters_fwd, float * parameters_nor) ;

extern float chisqr_mu ;
extern int fit_curve ;


DP G_Function_mu(DP x) // x is trial mu
{
	float parameters_nor[ MAX_PARAMETERS ];
	float mu, lambda, beta, nu, zeta[ MAX_PARAMETERS ];
	float rval;
	int i;
		
	if (error_sm_fit != 0) return 0;

	rval = 0;
	InverseTransform_all( brent_all, parameters_nor) ; 
	zeta[0] = -30;
	for (i = 0; i < R_sm_fit; i++) zeta[i+1] = parameters_nor [i + 2];
	zeta[R_sm_fit+1] = +30;

	lambda = parameters_nor[ 0 ] ;
	beta = parameters_nor[ 1 ] ;
	float xf = x ;
	InverseTransform_mu(&xf, &mu);
	nu = beta_to_nu( beta, mu );
	
	switch( fit_curve ) {
		case FIT_CURVE_ROC:
			rval = chisqr_ROC(mu, lambda, nu, R_sm_fit, zeta);
			break ;
		default:
			rval = chisqr(1, mu, lambda, nu, R_sm_fit, zeta);
			break ;
	}

	return rval;

}


// LL minimization does not give good fits on sumulated data; WHY??
// TO_FIT_FLAG determines whether mu is found by fittng to ROC, AFROC or ROC data points

float find_mu_minimize_G_brent(float *parameters_fwd, int n_parameters)
{
float mu_fwd, mu;
DP (*G_Function)(DP);//, float (*f)(float);
int i, nmin=0;
DP ax = 0,bx = 0,cx,fa,fb,fc,xmin,bren;

	for (i=0; i<n_parameters; i++)	brent_all[i] = parameters_fwd[i];

	float axf = ax, bxf = bx ;
	axf = (float) 0.2; ForwardTransform_mu(&axf, &mu_fwd);ax = mu_fwd;
	bxf = (float) 0.5; ForwardTransform_mu(&bxf, &mu_fwd);bx = mu_fwd;

	G_Function = &G_Function_mu;	

	NR::mnbrak(ax,bx,cx,fa,fb,fc,G_Function);
	if (error_sm_fit != 0) return(float)  0;

	bren=NR::brent(ax,bx,cx,G_Function,(float) TOL,xmin);
	if (error_sm_fit != 0) return (float) 0;

	if( !_finite( xmin ) ) {
		error_sm_fit = error_sm_fit + 2;
		mu = 0 ;
		return mu;
	}

	chisqr_mu = bren ;
	
	float xmf = xmin ;
	InverseTransform_mu(&xmf, &mu);

	return mu;
}



float find_mu_minimize_G_OneDimMinimizer(float *parameters_fwd, int n_parameters)
{
	for( int i = 0 ; i < n_parameters ; i++ ) brent_all[ i ] = parameters_fwd[ i ] ;

	float x1 = 0.1f, x2 = 10.0f ;
	float f = (float)G_Function_mu( x1 ) ;
	float fmid = (float)G_Function_mu( x2 ) ;
	/*
	if( f * fmid >= 0 ) {
		chisqr_mu = (float)G_Function_mu( (double)x2 ) ;
		return 10.0f ;
	}
	*/
	float xmid ;
	float rtbis, dx ;
	if( f < fmid ) {
		rtbis = x1 ;
		dx = x2 - x1 ;
	} else {
		rtbis = x2 ;
		dx = x1 - x2 ;
	}
	int count = 0 ;
	float xmid_old = 0.0f, fmid_old = 0.0f ;

	while( 1 ) {
		count++ ;
		if( count > 500 ) break ;
		dx *= 0.5f ;
		xmid = rtbis + dx ;
		fmid = (float)G_Function_mu( (double)xmid ) ;
		if( fmid < fmid_old ) rtbis = xmid ;
		if( fabs( dx ) <= 1e-6 ) break ;
		xmid_old = xmid ;
		fmid_old = fmid ;
	}

	chisqr_mu = fmid ;
	return xmid ;

}


float find_mu_minimize_G(float *parameters_fwd, int n_parameters)
{
	float ret = find_mu_minimize_G_brent( parameters_fwd, n_parameters ) ;
	/*
	float ret2 ;
	if( ret >= 5.0f ) {
		ret2 = find_mu_minimize_G_OneDimMinimizer( parameters_fwd, n_parameters ) ;
		if( ret2 < 5.0f ) ret = ret2 ;
	}
	*/
	if( ret >= 6.0f ) ret = 6.0f ;
	return ret ;
}