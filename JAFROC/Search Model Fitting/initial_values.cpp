#include "stdafx.h"
#include "../common.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


float ***parameters_ROC = NULL ;
int **n_parameters_ROC ;
float ***parameters_AFROC = NULL ;
int **n_parameters_AFROC ;
float ***parameters_FROC = NULL ;
int **n_parameters_FROC ;


// defined in chisqr.cpp
#define MAX_PARAMETERS 30

// FindZeta_Minimize_G
float find_zeta_minimize_G(float lambda) ;
float find_zeta_minimize_G_FROC(float lambda) ;


void init_parameters_roc( void )
{
	if( parameters_ROC != NULL ) clear_parameters_roc() ;

	parameters_ROC = new float **[ N_TREATMENTS ] ;
	n_parameters_ROC = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		parameters_ROC[ i ] = new float *[ MAX_READERS ] ;
		n_parameters_ROC[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			parameters_ROC[ i ][ j ] = new float[ 2 + number_of_bins_ROC[ i ][ j ] ] ;
			n_parameters_ROC[ i ][ j ] = 2 + number_of_bins_ROC[ i ][ j ] ;
		}
	}
}


void clear_parameters_roc( void )
{
	if( parameters_ROC == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] parameters_ROC[ i ][ j ] ;
		}
		delete[] parameters_ROC[ i ] ;
		delete[] n_parameters_ROC[ i ] ;
	}
	delete[] parameters_ROC ;
	delete[] n_parameters_ROC ;

	parameters_ROC = NULL ;
}


void find_zeta_roc_afroc( float lambda, int *fp_bin, float *zeta, int n_bins )
{
	for( int i = 0 ; i < n_bins ; i++ ) {
		float fp_bin_sum = 0.0f ;
		for( int l = i ; l < n_bins ; l++ ) fp_bin_sum += fp_bin[ l ] ;
		FP_sm_fit[ 0 ] = fp_bin_sum ;
		zeta[ i ] = find_zeta_minimize_G( lambda ) ;
	}		
}


void find_zeta_froc( float lambda, int *nl_bin, float *zeta, int n_bins )
{
	for( int i = 0 ; i < n_bins ; i++ ) {
		float nl_bin_sum = 0.0f ;
		for( int l = i ; l < n_bins ; l++ ) nl_bin_sum += nl_bin[ l ] ;
		NL_sm_fit[ 0 ] = nl_bin_sum ;
		zeta[ i ] = find_zeta_minimize_G_FROC( lambda ) ;
		if( i == 0 && zeta[ i ] < -3.0f ) zeta[ i ] = -3.0f ;
		if( i > 0 && zeta[ i ] < zeta[ i - 1 ] ) {
			zeta[ i ] = zeta[ i - 1 ] + 0.1 ;
		}
	}
}


void initial_values_from_roc_data( void )
{
	bin_and_cumulate_roc_data() ;
	init_parameters_roc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int c1 = 0 ;
			for( int l = 0 ; l < MAX_CTFFS ; l++ ) if( FP_BIN_ROC[ i ][ j ][ l ] != UNINITIALIZED ) c1++ ;
			if( c1 != 0 ) {
				int *FP_BIN_ROC_ij = FP_BIN_ROC[ i ][ j ] ;
				int **TP_BIN_ROC_ij = TP_BIN_ROC[ i ][ j ] ;

				float *FPF_BIN_ROC_ij = FPF_BIN_ROC[ i ][ j ] ;
				float *TPF_BIN_ROC_ij = TPF_BIN_ROC[ i ][ j ] ;
				int n_bins = number_of_bins_ROC[ i ][ j ] ;
				
				float lambda ;
				if( FPF_BIN_ROC_ij[ n_bins - 1 ] == 1.0 )
					lambda = -2.0f * log( 1.0f - 0.99f ) ;
				else if( FPF_BIN_ROC_ij[ n_bins - 1 ] == 0.0 )
					lambda = -2.0f * log( 1.0f - 0.01f ) ;
				else
					lambda = -2.0f * log( 1.0f - FPF_BIN_ROC_ij[ n_bins - 1 ] ) ;
				
				float beta = 1.0f ;
				float *zeta = new float[ n_bins ] ;

				find_zeta_roc_afroc( lambda, FP_BIN_ROC_ij, zeta, n_bins ) ;

				parameters_ROC[ i ][ j ][ 0 ] = lambda ;
				parameters_ROC[ i ][ j ][ 1 ] = beta ;
				for( int l = 0 ; l < n_bins ; l++ ) parameters_ROC[ i ][ j ][ 2 + l ] = zeta[ l ] ;

				delete[] zeta ;
			}
		}
	}
}


void init_parameters_afroc( void )
{
	if( parameters_AFROC != NULL ) clear_parameters_afroc() ;

	parameters_AFROC = new float **[ N_TREATMENTS ] ;
	n_parameters_AFROC = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		parameters_AFROC[ i ] = new float *[ MAX_READERS ] ;
		n_parameters_AFROC[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			parameters_AFROC[ i ][ j ] = new float[ 2 + number_of_bins_AFROC[ i ][ j ] ] ;
			n_parameters_AFROC[ i ][ j ] = 2 + number_of_bins_AFROC[ i ][ j ] ;
		}
	}
}


void clear_parameters_afroc( void )
{
	if( parameters_AFROC == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] parameters_AFROC[ i ][ j ] ;
		}
		delete[] parameters_AFROC[ i ] ;
		delete[] n_parameters_AFROC[ i ] ;
	}
	delete[] parameters_AFROC ;
	delete[] n_parameters_AFROC ;

	parameters_AFROC = NULL ;
}


void initial_values_from_afroc_data( void )
{
	bin_and_cumulate_afroc_data() ;
	init_parameters_afroc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int c1 = 0 ;
			for( int l = 0 ; l < MAX_CTFFS ; l++ ) if( FP_BIN_AFROC[ i ][ j ][ l ] != UNINITIALIZED ) c1++ ;
			if( c1 != 0 ) {
				int *FP_BIN_AFROC_ij = FP_BIN_AFROC[ i ][ j ] ;
				int *LL_BIN_AFROC_ij = LL_BIN_AFROC[ i ][ j ] ;

				float *FPF_BIN_AFROC_ij = FPF_BIN_AFROC[ i ][ j ] ;
				float *LLF_BIN_AFROC_ij = LLF_BIN_AFROC[ i ][ j ] ;
				int n_bins = number_of_bins_AFROC[ i ][ j ] ;
				
				float lambda ;
				if( FPF_BIN_AFROC_ij[ n_bins - 1 ] == 1.0 ) 
					lambda = -2.0f * log( 1.0f - 0.99f ) ;
				else if( FPF_BIN_AFROC_ij[ n_bins - 1 ] == 0.0 )
					lambda = -2.0f * log( 1.0f - 0.01f ) ;
				else
					lambda = -2.0f * log( 1.0f - FPF_BIN_AFROC_ij[ n_bins - 1 ] ) ;

				float beta = 1.0f ;
				float *zeta = new float[ n_bins ] ;

				find_zeta_roc_afroc( lambda, FP_BIN_AFROC_ij, zeta, n_bins ) ;

				parameters_AFROC[ i ][ j ][ 0 ] = lambda ;
				parameters_AFROC[ i ][ j ][ 1 ] = beta ;
				for( int l = 0 ; l < n_bins ; l++ ) parameters_AFROC[ i ][ j ][ 2 + l ] = zeta[ l ] ;

				delete[] zeta ;
			}
		}
	}
}



void init_parameters_froc( void )
{
	if( parameters_FROC != NULL ) clear_parameters_froc() ;

	parameters_FROC = new float **[ N_TREATMENTS ] ;
	n_parameters_FROC = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		parameters_FROC[ i ] = new float *[ MAX_READERS ] ;
		n_parameters_FROC[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			parameters_FROC[ i ][ j ] = new float[ 2 + number_of_bins_FROC[ i ][ j ] ] ;
			n_parameters_FROC[ i ][ j ] = 2 + number_of_bins_FROC[ i ][ j ] ;
		}
	}
}


void clear_parameters_froc( void )
{
	if( parameters_FROC == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] parameters_FROC[ i ][ j ] ;
		}
		delete[] parameters_FROC[ i ] ;
		delete[] n_parameters_FROC[ i ] ;
	}
	delete[] parameters_FROC ;
	delete[] n_parameters_FROC ;

	parameters_FROC = NULL ;
}


void initial_values_from_froc_data( void )
{
	bin_and_cumulate_froc_data() ;
	init_parameters_froc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int c1 = 0 ;
			for( int l = 0 ; l < MAX_CTFFS ; l++ ) if( FP_BIN_FROC[ i ][ j ][ l ] != UNINITIALIZED ) c1++ ;
			if( c1 != 0 ) {
				int *NL_BIN_FROC_ij = NL_BIN_FROC[ i ][ j ] ;
				int *LL_BIN_FROC_ij = LL_BIN_FROC[ i ][ j ] ;

				float *FPF_BIN_FROC_ij = FPF_BIN_FROC[ i ][ j ] ;
				float *LLF_BIN_FROC_ij = LLF_BIN_FROC[ i ][ j ] ;
				int n_bins = number_of_bins_FROC[ i ][ j ] ;
				
				float lambda ;
				if( FPF_BIN_FROC_ij[ n_bins - 1 ] == 1.0 )
					lambda = -2.0f * log( 1.0f - 0.99f ) ;
				else if( FPF_BIN_FROC_ij[ n_bins - 1 ] == 0.0 ) 
					lambda = -2.0f * log( 1.0f - 0.01f ) ;
				else
					lambda = -2.0f * log( 1.0f - FPF_BIN_FROC_ij[ n_bins - 1 ] ) ;

				float beta = 1.0f ;
				float *zeta = new float[ n_bins ] ;

				find_zeta_froc( lambda, NL_BIN_FROC_ij, zeta, n_bins ) ;

				parameters_FROC[ i ][ j ][ 0 ] = lambda ;
				parameters_FROC[ i ][ j ][ 1 ] = beta ;
				for( int l = 0 ; l < n_bins ; l++ ) parameters_FROC[ i ][ j ][ 2 + l ] = zeta[ l ] ;

				delete[] zeta ;
			}
		}
	}
}
