#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <float.h>
#include <nr.h>
#include <nrutil.h>


#define FTOL 1.0E-6


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



DP funk( Vec_I_DP &param ) ;
extern int funk_len ;


int run_amoeba( float *parameters, int n_parameters )
{
	int i, iter, j, ndim = n_parameters ;

	int NP = n_parameters ;
	funk_len = NP ;
	int MP = NP + 1 ;
	float temptr_org = 1.0f ;

	Mat_IO_DP p( MP, NP ) ;
	Vec_IO_DP x( NP ) ;
	Vec_IO_DP y( MP ) ;

	for( i = 0 ; i < MP ; i++ )
		for( j = 0 ; j < NP ; j++ ) p[ i ][ j ] = 0.0 ;

	for( j = 0 ; j < NP ; j++ ) {
		p[ 0 ][ j ] = parameters[ j ] ;
	}
	for( j = 1 ; j < MP ; j++ ) p[ j ][ j - 1 ] = 0.1 ;

	for( j = 0 ; j < NP ; j++ ) x[ j ] = parameters[ j ] ;
	
	for( i = 0 ; i < MP ; i++ ) {
		for( j = 0 ; j < NP ; j++ ) x[ j ] = p[ i ][ j ] ;
		y[ i ] = funk( x ) ;
	}

	NR::amoeba( p, y, FTOL, funk, iter ) ;

	for( j = 0 ; j < NP ; j++ ) parameters[ j ] = (float)p[ 0 ][ j ] ;

	return 0;
}
