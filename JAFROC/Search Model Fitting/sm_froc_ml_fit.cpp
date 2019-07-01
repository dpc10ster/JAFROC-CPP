#include "stdafx.h"
#include "../common.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float ***froc_fit_parms = NULL ;
int **froc_fit_errors ;
float **froc_fit_avg_parms ;

extern int error_sm_fit ;


void init_froc_fit_parms( void )
{
	if( froc_fit_parms != NULL ) clear_froc_fit_parms() ;

	froc_fit_parms = new float **[ N_TREATMENTS ] ;
	froc_fit_errors = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		froc_fit_parms[ i ] = new float *[ MAX_READERS ] ;
		froc_fit_errors[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			froc_fit_parms[ i ][ j ] = new float[ 4 ] ;	// lambda, beta, mu, nu
			froc_fit_errors[ i ][ j ] = 0 ;
		}
	}

	froc_fit_avg_parms = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		froc_fit_avg_parms[ i ] = new float[ 4 ] ;
	}
}


void clear_froc_fit_parms( void )
{
	if( froc_fit_parms == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] froc_fit_parms[ i ][ j ] ;
		}
		delete[] froc_fit_parms[ i ] ;
		delete[] froc_fit_errors[ i ] ;
	}
	delete[] froc_fit_parms ;
	delete[] froc_fit_errors ;

	froc_fit_parms = NULL ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] froc_fit_avg_parms[ i ] ;
	}
	delete[] froc_fit_avg_parms ;
}


bool sm_froc_ml_fit( char *filename )
{
	initial_values_from_froc_data() ;
	init_froc_fit_parms() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int *FP_BIN_FROC_ij = FP_BIN_FROC[ i ][ j ] ;
			int *NL_BIN_FROC_ij = NL_BIN_FROC[ i ][ j ] ;
			int *LL_BIN_FROC_ij = LL_BIN_FROC[ i ][ j ] ;
			int *TP_BIN_FROC_ij = TP_BIN_FROC[ i ][ j ] ;

			int *NL1 = NL_BIN_FROC[ i ][ j ] ;
			int *LL1 = LL_BIN_FROC[ i ][ j ] ;

			int R = number_of_bins_FROC[ i ][ j ] ;
			int n_parameters = R + 2 ;
			n_parameters_FROC[ i ][ j ] = n_parameters ;
			int fit_method = FIT_METHOD ;

			float mu, chisqr ;

			char str[ 200 ] ;
			sprintf_s( str, "Perform FROC curve fitting for reader: %s modality: %s\r\n", readerTable[ j ], modalityTable[ i ] ) ;
			WriteToConsole( str ) ;
 			constrained_fit_froc( NL1, LL1, &mu, &chisqr, parameters_FROC[ i ][ j ], n_parameters_FROC[ i ][ j ], fit_method ) ;

			if( error_sm_fit != 0 ) {
				froc_fit_errors[ i ][ j ] = 1 ;
				continue ;
			}
/*
			float df = 2.0f * ( (float)R + 1.0f ) - 2.0f - (float)R - 1.0f ;
			float p_val ;
			if( df != 0.0f )
				p_val = 1.0 - chisqr_pdf( chisqr, df ) ;
*/
			float lambda = parameters_FROC[ i ][ j ][ 0 ] ;
			float beta = parameters_FROC[ i ][ j ][ 1 ] ;
			float nu = beta_to_nu( beta, mu ) ;

			froc_fit_parms[ i ][ j ][ 0 ] = lambda ;
			froc_fit_parms[ i ][ j ][ 1 ] = beta ;
			froc_fit_parms[ i ][ j ][ 2 ] = mu ;
			froc_fit_parms[ i ][ j ][ 3 ] = nu ;

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
					parms_temp0[ c1 ] = froc_fit_parms[ i ][ j ][ 0 ] ;
					parms_temp1[ c1 ] = froc_fit_parms[ i ][ j ][ 1 ] ;
					parms_temp2[ c1 ] = froc_fit_parms[ i ][ j ][ 2 ] ;
					parms_temp3[ c1 ] = froc_fit_parms[ i ][ j ][ 3 ] ;
					c1++ ;
				}
			}
			froc_fit_avg_parms[ i ][ 0 ] = mean( parms_temp0, c1 ) ;	
			froc_fit_avg_parms[ i ][ 1 ] = mean( parms_temp1, c1 ) ;
			froc_fit_avg_parms[ i ][ 2 ] = mean( parms_temp2, c1 ) ;
			froc_fit_avg_parms[ i ][ 3 ] = mean( parms_temp3, c1 ) ;

			delete[] parms_temp0 ;
			delete[] parms_temp1 ;
			delete[] parms_temp2 ;
			delete[] parms_temp3 ;
		}
	}
	return true ;
}
