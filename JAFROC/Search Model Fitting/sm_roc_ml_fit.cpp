#include "stdafx.h"
#include "../common.h"
#include <math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int MAX_L, MAX_CASES_LL[10];

float ***roc_fit_parms = NULL ;
int **roc_fit_errors ;
float **roc_fit_avg_parms ;

extern int error_sm_fit ;

void init_roc_fit_parms( void )
{
	if( roc_fit_parms != NULL ) clear_roc_fit_parms() ;

	roc_fit_parms = new float **[ N_TREATMENTS ] ;
	roc_fit_errors = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		roc_fit_parms[ i ] = new float *[ MAX_READERS ] ;
		roc_fit_errors[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			roc_fit_parms[ i ][ j ] = new float[ 5 ] ;	// lambda, beta, mu, nu
			roc_fit_errors[ i ][ j ] = 0 ;
		}
	}
	roc_fit_avg_parms = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		roc_fit_avg_parms[ i ] = new float[ 5 ] ;
	}
}


void clear_roc_fit_parms( void )
{
	if( roc_fit_parms == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] roc_fit_parms[ i ][ j ] ;
		}
		delete[] roc_fit_parms[ i ] ;
		delete[] roc_fit_errors[ i ] ;
	}
	delete[] roc_fit_parms ;
	delete[] roc_fit_errors ;

	roc_fit_parms = NULL ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] roc_fit_avg_parms[ i ] ;
	}
	delete[] roc_fit_avg_parms ;
}



bool sm_roc_ml_fit( char *filename )
{
	initial_values_from_roc_data() ;
	init_roc_fit_parms() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int *FP_BIN_ROC_ij = FP_BIN_ROC[ i ][ j ] ;
			int *NL_BIN_ROC_ij = NL_BIN_ROC[ i ][ j ] ;
			int *LL_BIN_ROC_ij = LL_BIN_ROC[ i ][ j ] ;
			int **TP_BIN_ROC_ij = TP_BIN_ROC[ i ][ j ] ;

			int *FP1 = FP_BIN_ROC[ i ][ j ] ;
			int **TP1 = TP_BIN_ROC[ i ][ j ] ;

			int R = number_of_bins_ROC[ i ][ j ] ;
			int n_parameters = R + 2 ;
			n_parameters_ROC[ i ][ j ] = n_parameters ;
			int fit_method = FIT_METHOD ;

			float mu, chisqr ;

			char str[ 200 ] ;
			sprintf_s( str, "Perform ROC curve fitting for reader: %s modality: %s\r\n", readerTable[ j ], modalityTable[ i ] ) ;
			WriteToConsole( str ) ;
			constrained_fit_roc( FP1, TP1, &mu, &chisqr, parameters_ROC[ i ][ j ], n_parameters_ROC[ i ][ j ], fit_method ) ;

			if( error_sm_fit != 0 ) {
				roc_fit_errors[ i ][ j ] = 1 ;
				continue ;
			}

			float lambda = parameters_ROC[ i ][ j ][ 0 ] ;
			float beta = parameters_ROC[ i ][ j ][ 1 ] ;
			float nu = beta_to_nu( beta, mu ) ;

			roc_fit_parms[ i ][ j ][ 0 ] = lambda ;
			roc_fit_parms[ i ][ j ][ 1 ] = beta ;
			roc_fit_parms[ i ][ j ][ 2 ] = mu ;
			roc_fit_parms[ i ][ j ][ 3 ] = nu ;
		}
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			float *parms_temp0 = new float[ MAX_READERS ] ;
			float *parms_temp1 = new float[ MAX_READERS ] ;
			float *parms_temp2 = new float[ MAX_READERS ] ;
			float *parms_temp3 = new float[ MAX_READERS ] ;
			int c1 = 0 ;
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( roc_fit_errors[ i ][ j ] == 0 ) {
					parms_temp0[ c1 ] = roc_fit_parms[ i ][ j ][ 0 ] ;
					parms_temp1[ c1 ] = roc_fit_parms[ i ][ j ][ 1 ] ;
					parms_temp2[ c1 ] = roc_fit_parms[ i ][ j ][ 2 ] ;
					parms_temp3[ c1 ] = roc_fit_parms[ i ][ j ][ 3 ] ;
					c1++ ;
				}
			}
			roc_fit_avg_parms[ i ][ 0 ] = mean( parms_temp0, c1 ) ;	
			roc_fit_avg_parms[ i ][ 1 ] = mean( parms_temp1, c1 ) ;
			roc_fit_avg_parms[ i ][ 2 ] = mean( parms_temp2, c1 ) ;
			roc_fit_avg_parms[ i ][ 3 ] = mean( parms_temp3, c1 ) ;

			delete[] parms_temp0 ;
			delete[] parms_temp1 ;
			delete[] parms_temp2 ;
			delete[] parms_temp3 ;
		}
	}
	return true ;
}
