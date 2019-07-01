#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"
#include "nr.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float gaussint( float x )
{
	return 1.0f - ( 0.5f - 0.5f * NR::erff( sqrt(2.0f)/2.0f * x ) ) ;
}

float gauss_pdf( float x )
{
	return gaussint( x ) ;
}

float bisect_pdf2( float a, float u, float l )
{
	int sa = 1 ;
	float del = 1.0e-6f ;
	float p = a ;
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
		z = gauss_pdf( mid ) ;
		count++ ;
	}
	return mid ;
}

float gauss_cvf( float p )
{
	if( p < 0.0f || p > 1.0f ) {
		return -1.0f ;
	}
	if( p == 0.0f ) return 1e12f ;
	if( p == 1.0f ) return -1e12f ;

	int adjust ;
	if( p > 0.5f ) {
		p = 1.0f - p ;
		adjust = 1 ;
	} else {
		adjust = 0 ;
	}

	float below = 0.0f ;
	float up = 1.0f ;

	while( gauss_pdf( up ) < 1.0f - p ) {
		below = up ;
		up = 2.0f * up ;
	}

	float x = bisect_pdf2( 1.0f - p, up, below ) ;
	if( adjust ) {
		p = 1.0f - p ;
		return -x ;
	} else {
		return x ;
	}

}


float ZtoP( float z )
{ 
	return 2.0f * ( 1.0f - gauss_pdf( z ) ) ;
}

float PtoZ( float p )
{
	return gauss_cvf( p / 2.0f ) ;
}

float Obs_Power( float p_value )
{
	float ref = PtoZ( 0.05f ) ;
	float z = PtoZ( p_value ) ;
	float lb = ref - z ;
	float power = 1.0f - gauss_pdf( lb ) ;
	return power ;
}

float fpf_poi( float zeta, float lambda )
{
	return 1.0f - exp( -HALF * lambda + HALF * lambda * NR::erff( HALF_ROOT_TWO * zeta ) ) ;
}

float nlf_poi( float zeta, float lambda )
{
	return lambda * ( 1.0f - gauss_pdf( zeta ) ) ;
}

float llf_poi( float zeta, float mu, float nu )
{
	return nu * ( 1.0f - gauss_pdf( zeta - mu ) ) ;
}

float inv_nlf_poi( float nlf, float lambda )
{
	return -gauss_cvf( 1.0f - nlf / lambda ) ;
}

float inv_llf_poi( float llf, float mu, float nu )
{
	return mu - gauss_cvf( 1.0f - llf / nu ) ;
}
/*
float tpf_poi_constant_lesion( float z, float mu, float lambda, float nu, float s )
{
	float rval =
		1.0f-exp(-lambda)+exp(-lambda)*(1.0f-pow((1.0f-nu+nu*(HALF*NR::erff(HALF_ROOT_TWO*(z-mu))+HALF)),s)
		-pow((1.0-HALF*nu+HALF*nu*NR::erff(HALF_ROOT_TWO*(z-mu))),s)
		*(exp(-HALF*lambda+HALF*lambda*NR::erff(HALF_ROOT_TWO*z))*exp(lambda)-1.0)) ;
	return rval ;
}
*/
/*
float tpf_poi( float z, float mu, float lambda, float nu )
{
	int max_lesions = maximum( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ;
	int *num_of_imgs_with_spec_num_of_lesions = new int[ max_lesions ] ;
	float *weights = new float[ max_lesions ] ;
	float rval = 0.0f ;

	for( int i = 1 ; i <= max_lesions ; i++ ) {
		int c1 = 0 ;
		for( int j = 0 ; j < MAX_CASES[ 1 ] ; j++ ) if( N_Lesions_Per_Image[ j ] == i ) c1++ ;
		num_of_imgs_with_spec_num_of_lesions[ i - 1 ] = c1 ;
	}
	for( int i = 1 ; i <= max_lesions ; i++ ) {
		weights[ i - 1 ] = (float)num_of_imgs_with_spec_num_of_lesions[ i - 1 ] / (float)MAX_CASES[ 1 ] ;
	}
	for( int i = 1 ; i <= max_lesions ; i++ ) {
		if( weights[ i - 1 ] == 0 ) continue ;
		rval += weights[ i - 1 ] * tpf_poi_constant_lesion( z, mu, lambda, nu, i ) ;
	}
	
	return rval ;
}
*/
typedef struct {
	float mu ;
	float lambda ;
	float nu ;
	float target ;
} inv_param ;
/*
float target_tpf_difference( inv_param a, float zeta )
{
	return a.target - tpf_poi( zeta, a.mu, a.lambda, a.nu ) ;
}
*/
float target_fpf_difference( inv_param a, float zeta )
{
	return a.target - fpf_poi( zeta, a.lambda ) ;
}

float OneDimMinimizer( float (*func)( inv_param a, float zeta ), inv_param a, float x1, float x2, float acc )
{
	float f = func( a, x1 ) ;
	float fmid = func( a, x2 ) ;

	if( f * fmid >= 0 ) {
		f = func( a, x2 ) ;
		return -1.0f ;
	}

	float rtbis ;
	float dx ;
	if( f < 0 ) {
		rtbis = x1 ;
		dx = x2 - x1 ;
	} else {
		rtbis = x2 ;
		dx = x1 - x2 ;
	}
	int count = 0 ;
	float xmid_old = 0.0f ;
	float fmid_old = 0.0f ;
	float xmid ;
	while( 1 ) {
		count++ ;
		if( count > 500 ) break ;
		dx = dx * 0.5f ;
		xmid = rtbis + dx ;
		fmid = func( a, xmid ) ;
		if( fmid < 0 ) rtbis = xmid ;
		if( fabs( dx ) < acc ) break ;
		xmid_old = xmid ;
		fmid_old = fmid ;
	}
	return xmid ;
}
/*
float inv_tpf_poi( float target_tpf, float mu, float lambda, float nu )
{
	inv_param a ;

	a.mu = mu ;
	a.lambda = lambda ;
	a.nu = nu ;
	a.target = target_tpf ;

	return OneDimMinimizer( target_tpf_difference, a, -20.0f, 20.0f, 1e-6 ) ;
}

float inv_fpf_poi( float target_fpf, float lambda )
{
	inv_param a ;

	a.lambda = lambda ;
	a.target = target_fpf ;

	return OneDimMinimizer( target_fpf_difference, a, -20.0f, 20.0f, 1e-6 ) ;
}
*/