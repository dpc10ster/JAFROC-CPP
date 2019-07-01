#include "stdafx.h"
#include "../common.h"
#include <memory.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int ***NL_BIN_ROC = NULL ;
int ***LL_BIN_ROC = NULL ;
int ***FP_BIN_ROC = NULL ;
int ****TP_BIN_ROC = NULL ;
int ***TP_BIN_ROC1 = NULL ;
float ***NLF_BIN_ROC = NULL ;
float ***LLF_BIN_ROC = NULL ;
float ***FPF_BIN_ROC = NULL ;
float ***TPF_BIN_ROC = NULL ;


void init_binned_roc_data( void )
{
	clear_binned_roc_data() ;

	NL_BIN_ROC = new int **[ N_TREATMENTS ] ;
	LL_BIN_ROC = new int **[ N_TREATMENTS ] ;
	FP_BIN_ROC = new int **[ N_TREATMENTS ] ;
	TP_BIN_ROC = new int ***[ N_TREATMENTS ] ;
	TP_BIN_ROC1 = new int **[ N_TREATMENTS ] ;
	NLF_BIN_ROC = new float **[ N_TREATMENTS ] ;
	LLF_BIN_ROC = new float **[ N_TREATMENTS ] ;
	FPF_BIN_ROC = new float **[ N_TREATMENTS ] ;
	TPF_BIN_ROC = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		NL_BIN_ROC[ i ] = new int *[ MAX_READERS ] ;
		LL_BIN_ROC[ i ] = new int *[ MAX_READERS ] ;
		FP_BIN_ROC[ i ] = new int *[ MAX_READERS ] ;
		TP_BIN_ROC[ i ] = new int **[ MAX_READERS ] ;
		TP_BIN_ROC1[ i ] = new int *[ MAX_READERS ] ;
		NLF_BIN_ROC[ i ] = new float *[ MAX_READERS ] ;
		LLF_BIN_ROC[ i ] = new float *[ MAX_READERS ] ;
		FPF_BIN_ROC[ i ] = new float *[ MAX_READERS ] ;
		TPF_BIN_ROC[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			NL_BIN_ROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			LL_BIN_ROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			FP_BIN_ROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			TP_BIN_ROC[ i ][ j ] = new int *[ MAX_LL ] ;
			TP_BIN_ROC1[ i ][ j ] = new int[ MAX_CTFFS ] ;
			NLF_BIN_ROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			LLF_BIN_ROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			FPF_BIN_ROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			TPF_BIN_ROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			for( int k = 0 ; k < MAX_CTFFS ; k++ ) {
				NL_BIN_ROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				LL_BIN_ROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				FP_BIN_ROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				TP_BIN_ROC1[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				NLF_BIN_ROC[ i ][ j ][ k ] = UNINITIALIZED ;
				LLF_BIN_ROC[ i ][ j ][ k ] = UNINITIALIZED ;
				FPF_BIN_ROC[ i ][ j ][ k ] = UNINITIALIZED ;
				TPF_BIN_ROC[ i ][ j ][ k ] = UNINITIALIZED ;
			}
			for( int l = 0 ; l < MAX_LL ; l++ ) {
				TP_BIN_ROC[ i ][ j ][ l ] = new int[ MAX_CTFFS ] ;
				for( int k = 0 ; k < MAX_CTFFS ; k++ ) {
					TP_BIN_ROC[ i ][ j ][ l ][ k ] = (int)UNINITIALIZED ;
				}
			}
		}
	}
}

void clear_binned_roc_data( void )
{
	if( NL_BIN_ROC != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int l = 0 ; l < MAX_LL ; l++ ) {
					delete[] TP_BIN_ROC[ i ][ j ][ l ] ;
				}
				delete[] NL_BIN_ROC[ i ][ j ] ;
				delete[] LL_BIN_ROC[ i ][ j ] ;
				delete[] FP_BIN_ROC[ i ][ j ] ;
				delete[] TP_BIN_ROC[ i ][ j ] ;
				delete[] TP_BIN_ROC1[ i ][ j ] ;
				delete[] NLF_BIN_ROC[ i ][ j ] ;
				delete[] LLF_BIN_ROC[ i ][ j ] ;
				delete[] FPF_BIN_ROC[ i ][ j ] ;
				delete[] TPF_BIN_ROC[ i ][ j ] ;
			}
			delete[] NL_BIN_ROC[ i ] ;
			delete[] LL_BIN_ROC[ i ] ;
			delete[] FP_BIN_ROC[ i ] ;
			delete[] TP_BIN_ROC[ i ] ;
			delete[] TP_BIN_ROC1[ i ] ;
			delete[] NLF_BIN_ROC[ i ] ;
			delete[] LLF_BIN_ROC[ i ] ;
			delete[] FPF_BIN_ROC[ i ] ;
			delete[] TPF_BIN_ROC[ i ] ;
		}
		delete[] NL_BIN_ROC ;
		delete[] LL_BIN_ROC ;
		delete[] FP_BIN_ROC ;
		delete[] TP_BIN_ROC ;
		delete[] TP_BIN_ROC1 ;
		delete[] NLF_BIN_ROC ;
		delete[] LLF_BIN_ROC ;
		delete[] FPF_BIN_ROC ;
		delete[] TPF_BIN_ROC ;

		NL_BIN_ROC = NULL ;
	}
}



void bin_and_cumulate_roc_data( void )
{
	convert_to_roc() ;
	find_optimal_cutoffs_roc() ;

	init_binned_roc_data() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			float zeta_temp[ MAX_CTFFS + 1 ] ;
			memcpy( zeta_temp, zeta_array_ROC[ i ][ j ], number_of_bins_ROC[ i ][ j ] * sizeof( float ) ) ;
			zeta_temp[ number_of_bins_ROC[ i ][ j ] ] = -UNINITIALIZED ;
			for( int t = 0 ; t < number_of_bins_ROC[ i ][ j ] ; t++ ) {
				NL_BIN_ROC[ i ][ j ][ t ] = array_count( NL[ i ][ j ], MAX_CASES[ 0 ] + MAX_CASES[ 1 ], MAX_NL, zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				LL_BIN_ROC[ i ][ j ][ t ] = array_count( LL[ i ][ j ], MAX_CASES[ 1 ], MAX_LL, zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				FP_BIN_ROC[ i ][ j ][ t ] = array_count( FP[ i ][ j ], MAX_CASES[ 0 ], zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				TP_BIN_ROC1[ i ][ j ][ t ] = array_count( TP[ i ][ j ], MAX_CASES[ 1 ], zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				for( int l = 0 ; l < MAX_LL ; l++ ) {
					int acc = 0 ;
					for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
						if( N_Lesions_Per_Image[ k ] == l + 1 ) {
							if( TP[ i ][ j ][ k ] > zeta_temp[ t ] && TP[ i ][ j ][ k ] <= zeta_temp[ t + 1 ] ) acc++ ;
						}
					}
					TP_BIN_ROC[ i ][ j ][ l ][ t ] = acc ;
				}
			}
		}
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int t = 0 ; t < number_of_bins_ROC[ i ][ j ] ; t++ ) {
				NLF_BIN_ROC[ i ][ j ][ t ] = 
					(float)total( &NL_BIN_ROC[ i ][ j ][ number_of_bins_ROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;
				LLF_BIN_ROC[ i ][ j ][ t ] = 
					(float)total( &LL_BIN_ROC[ i ][ j ][ number_of_bins_ROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)( total( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
				FPF_BIN_ROC[ i ][ j ][ t ] = 
					(float)total( &FP_BIN_ROC[ i ][ j ][ number_of_bins_ROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)MAX_CASES[ 0 ] ;
				TPF_BIN_ROC[ i ][ j ][ t ] = 
					(float)total( &TP_BIN_ROC1[ i ][ j ][ number_of_bins_ROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)MAX_CASES[ 1 ] ;
			}
		}
	}
}
