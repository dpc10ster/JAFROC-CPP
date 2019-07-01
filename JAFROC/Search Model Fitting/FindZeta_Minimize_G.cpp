#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <float.h>
#include <nr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


float brent_lambda ;
float chisqr_zeta ;
extern int error ;


DP G_Function_zeta(DP x) // x is trial zeta
{
float mu = 0, nu = 0;

	float zeta[ 3 ];
	float rval;
		
	if (error != 0) return 0;

	rval = 0;
		
	zeta[0] = -30;
	zeta[1] = (float)x;
	zeta[2] = +30;

	rval = chisqr (0, mu, brent_lambda, nu, 1, zeta);

	if( !_finite( rval ) ) {
		error = error + 32;
		rval = 0 ;
	}

	return rval;

}




float find_zeta_minimize_G(float lambda)
{
DP (*G_Function)(DP);
int nmin=0;
DP ax,bx,cx,fa,fb,fc,xmin,bren;

	brent_lambda = lambda;

	ax = (float) 0.2; 
	bx = (float) 0.31; 

	G_Function = &G_Function_zeta;	

	NR::mnbrak(ax,bx,cx,fa,fb,fc,G_Function);
	if (error != 0) return(float)  0;

	bren=NR::brent(ax,bx,cx,G_Function,(float) TOL,xmin);
	if (error != 0) return (float) 0;

	if( !_finite( xmin ) ) {
		error = error + 2;
		xmin = -30 ;
	}

	chisqr_zeta = bren ;

	return xmin;


}



DP G_Function_zeta_FROC(DP x) // x is trial zeta
{
float mu = 0, nu = 0;

	float zeta[ 3 ];
	float rval;
		
	if (error != 0) return 0;

	rval = 0;
		
	zeta[0] = -30;
	zeta[1] = (float)x;
	zeta[2] = +30;

	rval = chisqr_FROC(0, mu, brent_lambda, nu, 1, zeta);

	if( !_finite( rval ) ) {
		error = error + 32;
		rval = 0 ;
	}

	return rval;

}




float find_zeta_minimize_G_FROC(float lambda)
{
DP (*G_Function)(DP);
int nmin=0;
DP ax,bx,cx,fa,fb,fc,xmin,bren;

	brent_lambda = lambda;

	ax = (float) 0.2; 
	bx = (float) 0.31; 

	G_Function = &G_Function_zeta_FROC;	

	NR::mnbrak(ax,bx,cx,fa,fb,fc,G_Function);
	if (error != 0) return(float)  0;

	bren=NR::brent(ax,bx,cx,G_Function,(float) TOL,xmin);
	if (error != 0) return (float) 0;

	if( !_finite( xmin ) ) {
		error = error + 2;
		xmin = -30 ;
	}

	chisqr_zeta = bren ;

	return xmin;


}
