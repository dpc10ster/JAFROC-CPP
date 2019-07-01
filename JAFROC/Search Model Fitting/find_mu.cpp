#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <nr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/*


#define TOL 1.0e-2
#define EQL 1.0e-4


float brent_all[ 3 ];
float qromb_parm[ 3 ];



float integrand_JAFROC_FOM (float z)
{
float pi = 3.1416f;
float ROOT_TWO, mu, lambda, nu;

	ROOT_TWO = pow(2, 0.5);
	mu = qromb_parm[ 0 ];
	lambda = qromb_parm[ 1 ];
	nu = qromb_parm[ 2 ];

	return 
		HALF*nu*(HALF-HALF*erff(HALF_ROOT_TWO*(z-mu)))*lambda*exp(-HALF*z*z
		-HALF*lambda+HALF*lambda*erff(HALF_ROOT_TWO*z))*ROOT_TWO/pow(pi,HALF);

}




float SM_AFROC_AREA(float mu, float lambda, float nu) 
{

float area;

	qromb_parm[ 0 ] = mu;
	qromb_parm[ 1 ] = lambda;
	qromb_parm[ 2 ] = nu;

	area = qromb(integrand_JAFROC_FOM,-20.0f, 20.0f);

	return area;
}



float Function_mu(float x) // x is trial mu
{
	float mu, lambda, nu, AFROC_area;
	float rval;
		
	rval = 0;

	mu = x;
	lambda = brent_all[ 0 ] ;
	nu = brent_all[ 1 ];
	AFROC_area = brent_all[ 2 ];

	rval = pow((AFROC_area - SM_AFROC_AREA(mu, lambda, nu)),2);

	return rval;

}





float find_mu( float lambda, float nu, float area, float *chisqr)
{
float mu_fwd, mu, (*G_Function)(float);//, float (*f)(float);
int i, nmin=0;
float ax,bx,cx,fa,fb,fc,xmin,bren;
	
	brent_all[0] = lambda;
	brent_all[1] = nu; 
	brent_all[2] = area; 

	ax = (float) 0.2; // trial mu
	bx = (float) 0.5;

	G_Function = &Function_mu; // find mu given lambda and nu to get desired AFROC area	

	mnbrak(&ax,&bx,&cx,&fa,&fb,&fc,G_Function);
	if (error != 0) return(float)  0;

	bren=brent(ax,bx,cx,G_Function,(float) TOL,&xmin);
	if (error != 0) return (float) 0;

	mu = xmin;

	if( !_finite( xmin ) ) {
		error = error + 128;
		mu = 0 ;
		return mu;
	}

	*chisqr = bren ;

	return mu;
}
*/