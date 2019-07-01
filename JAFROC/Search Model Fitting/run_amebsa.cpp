#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <float.h>
#include <nr.h>
#include <nrutil.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define FTOL 1.0E-6
#define N 4
#define RAD 0.3
#define AUG 2.0


int idum=(-64);
int funk_len ;
extern int fit_curve ;

float LL_ROC_POI( float *parameters_fwd ) ;
float LL_AFROC_POI( float *parameters_fwd ) ;
float LL_FROC_POI( float *parameters_fwd ) ;


DP funk( Vec_I_DP &param )
{
	int i ;
	float parameters_fwd[ MAX_PARAMETERS ];
	float ret ;

//	for( i = 0 ; i < MAX_PARAMETERS ; i++ ) {
	for( i = 0 ; i < funk_len ; i++ ) {
		parameters_fwd[ i ] = (float)param[ i ] ;
	}

	switch( fit_curve ) {
		case FIT_CURVE_ROC:
			ret = LL_ROC_POI( parameters_fwd ) ;
			break ;
		case FIT_CURVE_AFROC:
			ret = LL_AFROC_POI( parameters_fwd ) ;
			break ;
		case FIT_CURVE_FROC:
			ret = LL_FROC_POI( parameters_fwd ) ;
			break ;
	}

	return ret ;
}


int run_amebsa( float *parameters, int n_parameters )
{
	int i, iiter, iter, j, ndim = n_parameters ;
	float temptr, ybb ;
	DP yb ;
//	float **p, *x, *y, *pb ;
	int NP = n_parameters ;
	funk_len = NP ;
	int MP = NP + 1 ;
	float temptr_org = 1.0f ;

	Mat_IO_DP p( MP, NP ) ;
	Vec_IO_DP x( NP ) ;
	Vec_IO_DP y( MP ) ;
	Vec_IO_DP pb( NP ) ;
//	p = matrix( 1, MP, 1, NP ) ;
//	x = vector( 1, NP ) ;
//	y = vector( 1, MP ) ;
//	pb = vector( 1, NP ) ;

	for( i = 0 ; i < MP ; i++ )
		for( j = 0 ; j < NP ; j++ ) p[ i ][ j ] = 0.0 ;

	for( j = 1 ; j < MP ; j++ ) p[ j ][ j - 1 ] = 1.0 ;
	for( i = 0 ; i < MP ; i++ ) {
		for( j = 0 ; j < NP ; j++ ) x[ j ] = ( p[ i ][ j ] += parameters[ j ] ) ;
		y[ i ] = funk( x ) ;
	}

	temptr = temptr_org ;
	iiter = 1000 ;
//	iiter = 200 ;

	yb = 1.0e30f ;
	ybb = 1.0e30f ;

//	for( jiter = 1 ; jiter <= 1000 ; jiter++ ) {
//	for( jiter = 1 ; jiter <= 1000 ; jiter++ ) {
	for(;;) {
		iter = iiter ;
		temptr /= 1.01 ;
//		temptr /= 1.05 ;
//		NR::amebsa( p, y, ndim, pb, &yb, FTOL, funk, &iter, temptr ) ;
		NR::amebsa( p, y, pb, yb, FTOL, funk, iter, temptr ) ;

		if( yb < ybb ) {
			ybb = yb ;
			for( j = 0 ; j < NP ; j++ ) parameters[ j ] = pb[ j ] ;
		}
		if (iter > 0) break;
		if( temptr < 1e-10 ) break ;
	}
	
//	free_vector(pb,1,NP);
//	free_vector(y,1,MP);
//	free_vector(x,1,NP);
//	free_matrix(p,1,MP,1,NP);
	return 0;
}
