#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <nr.h>



float chisqr_pdf( float x, float df )
{
	float temp = x ;
	if( temp < 0.0f ) temp = 0.0f ;
	float gres = (float)NR::gammp( df / 2.0f, temp / 2.0f ) ;
	return gres ;
}


double chisqr_pdf( double x, double df )
{
	double temp = x ;
	if( temp < 0.0 ) temp = 0.0 ;
	double gres = NR::gammp( df / 2.0, temp / 2.0 ) ;
	return gres ;
}