#include "stdafx.h"
#include "../common.h"
#include <stdio.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


float ***afroc_fit_parms = NULL ;
int **afroc_fit_errors ;
float **afroc_fit_avg_parms ;

extern int error_sm_fit ;

void init_afroc_fit_parms( void )
{
	if( afroc_fit_parms != NULL ) clear_afroc_fit_parms() ;

	afroc_fit_parms = new float **[ N_TREATMENTS ] ;
	afroc_fit_errors = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		afroc_fit_parms[ i ] = new float *[ MAX_READERS ] ;
		afroc_fit_errors[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			afroc_fit_parms[ i ][ j ] = new float[ 4 ] ;	// lambda, beta, mu, nu
			afroc_fit_errors[ i ][ j ] = 0 ;
		}
	}

	afroc_fit_avg_parms = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		afroc_fit_avg_parms[ i ] = new float[ 4 ] ;
	}
}


void clear_afroc_fit_parms( void )
{
	if( afroc_fit_parms == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] afroc_fit_parms[ i ][ j ] ;
		}
		delete[] afroc_fit_parms[ i ] ;
		delete[] afroc_fit_errors[ i ] ;
	}
	delete[] afroc_fit_parms ;
	delete[] afroc_fit_errors ;

	afroc_fit_parms = NULL ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] afroc_fit_avg_parms[ i ] ;
	}
	delete[] afroc_fit_avg_parms ;
}


bool sm_afroc_ml_fit( char *filename )
{
	initial_values_from_afroc_data() ;
	init_afroc_fit_parms() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int *FP_BIN_AFROC_ij = FP_BIN_AFROC[ i ][ j ] ;
			int *NL_BIN_AFROC_ij = NL_BIN_AFROC[ i ][ j ] ;
			int *LL_BIN_AFROC_ij = LL_BIN_AFROC[ i ][ j ] ;
			int *TP_BIN_AFROC_ij = TP_BIN_AFROC[ i ][ j ] ;

			int *FP1 = FP_BIN_AFROC[ i ][ j ] ;
			int *LL1 = LL_BIN_AFROC[ i ][ j ] ;

			int R = number_of_bins_AFROC[ i ][ j ] ;
			int n_parameters = R + 2 ;
			n_parameters_AFROC[ i ][ j ] = n_parameters ;
			int fit_method = FIT_METHOD ;

			float mu, chisqr ;

			char str[ 200 ] ;
			sprintf_s( str, "Perform AFROC curve fitting for reader: %s modality: %s\r\n", readerTable[ j ], modalityTable[ i ] ) ;
			WriteToConsole( str ) ;
			constrained_fit_afroc( FP1, LL1, &mu, &chisqr, parameters_AFROC[ i ][ j ], n_parameters_AFROC[ i ][ j ], fit_method ) ;

			if( error_sm_fit != 0 ) {
				afroc_fit_errors[ i ][ j ] = 1 ;
				continue ;
			}
/*
			float df = 2.0f * ( (float)R + 1.0f ) - 2.0f - (float)R - 1.0f ;
			float p_val ;
			if( df != 0.0f )
				p_val = 1.0 - chisqr_pdf( chisqr, df ) ;
*/
//			float lambda = parameters[ 0 ] ;
//			float beta = parameters[ 1 ] ;
			float lambda = parameters_AFROC[ i ][ j ][ 0 ] ;
			float beta = parameters_AFROC[ i ][ j ][ 1 ] ;
			float nu = beta_to_nu( beta, mu ) ;

			afroc_fit_parms[ i ][ j ][ 0 ] = lambda ;
			afroc_fit_parms[ i ][ j ][ 1 ] = beta ;
			afroc_fit_parms[ i ][ j ][ 2 ] = mu ;
			afroc_fit_parms[ i ][ j ][ 3 ] = nu ;
/*
		SHOW_INDIVIDUAL_AFROC_CURVE, mu , lambda, nu, FPF1, LLF1, rms_error
		sm_auc_ij = SM_AUC (mu, lambda, nu, N_LESIONS_PER_IMAGE)
		print, mu, lambda, nu, 0.0, sm_auc_ij, init_LL, final_LL, chisqr, df, p_val, rms_error
		SAVE_SM_AFROC_CURVE, mu , lambda, nu, FPF1, LLF1, DATA_FN, i, j
*/			
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
					parms_temp0[ c1 ] = afroc_fit_parms[ i ][ j ][ 0 ] ;
					parms_temp1[ c1 ] = afroc_fit_parms[ i ][ j ][ 1 ] ;
					parms_temp2[ c1 ] = afroc_fit_parms[ i ][ j ][ 2 ] ;
					parms_temp3[ c1 ] = afroc_fit_parms[ i ][ j ][ 3 ] ;
					c1++ ;
				}
			}
			afroc_fit_avg_parms[ i ][ 0 ] = mean( parms_temp0, c1 ) ;	
			afroc_fit_avg_parms[ i ][ 1 ] = mean( parms_temp1, c1 ) ;
			afroc_fit_avg_parms[ i ][ 2 ] = mean( parms_temp2, c1 ) ;
			afroc_fit_avg_parms[ i ][ 3 ] = mean( parms_temp3, c1 ) ;

			delete[] parms_temp0 ;
			delete[] parms_temp1 ;
			delete[] parms_temp2 ;
			delete[] parms_temp3 ;
		}
	}
	return true ;
}
