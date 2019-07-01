#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"
#include <math.h>
#include "nr.h"



// functions translated by hjy, expression from IDL routine using betai function in NR
float f_pdf( float x, float dfn, float dfd )
{
	float temp = x ;
	if( temp < 0.0f ) temp = 0.0f ;
	return 1.0f - (float)NR::betai( dfd / 2.0f, dfn / 2.0f, 1.0 * dfd / ( dfd + dfn * temp ) ) ;
}


// support functions
float bisect_pdf( float a[ 2 ], float u, float l )
{
	int sa = 2 ;
	float del = 1.0e-6f ;
	float p = a[ 0 ] ;
	if( p < 0.0f || p > 1.0f ) return -1.0f ;
	
	float up = u ;
	float low = l ;
	float mid = l + ( u - l ) * p ;
	int count = 1 ;
	int first = 1 ;
	float z ;
	while( fabs( up - low ) > del * mid && ( count < 100 ) ) {
		if( first == 1 ) {
			first = 0 ;
		} else {
			if( z > p ) up = mid ;
			else low = mid ;
			mid = ( up + low ) / 2.0f ;
		}
		z = t_pdf( mid, a[ 1 ] ) ;
		count++ ;
	}
	return mid ;
}


float t_cvf( float a1, float df )
{
	float a = a1 ;
	if( a < 0.0f || a > 1.0f ) {
		return 0.0f ;
	}
	int adjust ;
	if( a > 0.5f ) {
		adjust = 1 ;
	} else {
		adjust = 0 ;
		a = 1.0f - a ;
	}
	if( a1 == 0.0f ) return 1.0e12f ;
	if( a1 == 1.0f ) return -1.0e12f ;

	float up, below ;
	if( df == 1.0f ) {
		up = 100.0f * 0.005f / a1 ;
		if( up < 100.0f ) up = 100.0f ;
	} else if( df == 2.0f ) {
		up = 10.0f * 0.005f / a1 ;
		if( up < 10.0f ) up = 10.0f ;
	} else if( df > 2.0f && df <= 5.0f ) {
		up = 5.0f * 0.005f / a1 ;
		if( up < 5.0f ) up = 5.0f ;
	} else if( df > 5.0f && df <= 14.0f ) {
		up = 4.0f * 0.005f / a1 ;
		if( up < 4.0f ) up = 4.0f ;
	} else {
		up = 3.0f * 0.005f / a1 ;
		if( up < 3.0f ) up = 3.0f ;
	}
	while( t_pdf( up, df ) < a ) {
		below = up ;
		up = 2.0f * up ;
	}

	float arg[ 2 ] ;
	arg[ 0 ] = a ;
	arg[ 1 ] = df ;

	float x = bisect_pdf( arg, up, 0.0f ) ;

	if( adjust ) {
		return -x ;
	} else {
		return x ;
	}
}


float t_pdf( float v, float df )
{
	float positive ;
	if( v > 0 ) positive = 1.0f ;
	else positive = 0.0f ;

	return positive - ( positive - 0.5f ) * (float)NR::betai( df / 2.0f, 0.5, df / ( df + v * v ) ) ;
}
