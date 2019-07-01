#include "stdafx.h"
#include "../common.h"
#include <memory.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int ***NL_BIN_AFROC = NULL ;
int ***LL_BIN_AFROC = NULL ;
int ***FP_BIN_AFROC = NULL ;
int ***TP_BIN_AFROC = NULL ;
float ***NLF_BIN_AFROC = NULL ;
float ***LLF_BIN_AFROC = NULL ;
float ***FPF_BIN_AFROC = NULL ;
float ***TPF_BIN_AFROC = NULL ;


void init_binned_afroc_data( void )
{
	clear_binned_afroc_data() ;

	NL_BIN_AFROC = new int **[ N_TREATMENTS ] ;
	LL_BIN_AFROC = new int **[ N_TREATMENTS ] ;
	FP_BIN_AFROC = new int **[ N_TREATMENTS ] ;
	TP_BIN_AFROC = new int **[ N_TREATMENTS ] ;
	NLF_BIN_AFROC = new float **[ N_TREATMENTS ] ;
	LLF_BIN_AFROC = new float **[ N_TREATMENTS ] ;
	FPF_BIN_AFROC = new float **[ N_TREATMENTS ] ;
	TPF_BIN_AFROC = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		NL_BIN_AFROC[ i ] = new int *[ MAX_READERS ] ;
		LL_BIN_AFROC[ i ] = new int *[ MAX_READERS ] ;
		FP_BIN_AFROC[ i ] = new int *[ MAX_READERS ] ;
		TP_BIN_AFROC[ i ] = new int *[ MAX_READERS ] ;
		NLF_BIN_AFROC[ i ] = new float *[ MAX_READERS ] ;
		LLF_BIN_AFROC[ i ] = new float *[ MAX_READERS ] ;
		FPF_BIN_AFROC[ i ] = new float *[ MAX_READERS ] ;
		TPF_BIN_AFROC[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			NL_BIN_AFROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			LL_BIN_AFROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			FP_BIN_AFROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			TP_BIN_AFROC[ i ][ j ] = new int[ MAX_CTFFS ] ;
			NLF_BIN_AFROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			LLF_BIN_AFROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			FPF_BIN_AFROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			TPF_BIN_AFROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			for( int k = 0 ; k < MAX_CTFFS ; k++ ) {
				NL_BIN_AFROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				LL_BIN_AFROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				FP_BIN_AFROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				TP_BIN_AFROC[ i ][ j ][ k ] = (int)UNINITIALIZED ;
				NLF_BIN_AFROC[ i ][ j ][ k ] = UNINITIALIZED ;
				LLF_BIN_AFROC[ i ][ j ][ k ] = UNINITIALIZED ;
				FPF_BIN_AFROC[ i ][ j ][ k ] = UNINITIALIZED ;
				TPF_BIN_AFROC[ i ][ j ][ k ] = UNINITIALIZED ;
			}
		}
	}
}

void clear_binned_afroc_data( void )
{
	if( NL_BIN_AFROC != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				delete[] NL_BIN_AFROC[ i ][ j ] ;
				delete[] LL_BIN_AFROC[ i ][ j ] ;
				delete[] FP_BIN_AFROC[ i ][ j ] ;
				delete[] TP_BIN_AFROC[ i ][ j ] ;
				delete[] NLF_BIN_AFROC[ i ][ j ] ;
				delete[] LLF_BIN_AFROC[ i ][ j ] ;
				delete[] FPF_BIN_AFROC[ i ][ j ] ;
				delete[] TPF_BIN_AFROC[ i ][ j ] ;
			}
			delete[] NL_BIN_AFROC[ i ] ;
			delete[] LL_BIN_AFROC[ i ] ;
			delete[] FP_BIN_AFROC[ i ] ;
			delete[] TP_BIN_AFROC[ i ] ;
			delete[] NLF_BIN_AFROC[ i ] ;
			delete[] LLF_BIN_AFROC[ i ] ;
			delete[] FPF_BIN_AFROC[ i ] ;
			delete[] TPF_BIN_AFROC[ i ] ;
		}
		delete[] NL_BIN_AFROC ;
		delete[] LL_BIN_AFROC ;
		delete[] FP_BIN_AFROC ;
		delete[] TP_BIN_AFROC ;
		delete[] NLF_BIN_AFROC ;
		delete[] LLF_BIN_AFROC ;
		delete[] FPF_BIN_AFROC ;
		delete[] TPF_BIN_AFROC ;

		NL_BIN_AFROC = NULL ;
	}
}



void bin_and_cumulate_afroc_data( void )
{
	convert_to_roc() ;
	find_optimal_cutoffs_afroc() ;

	init_binned_afroc_data() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			float zeta_temp[ MAX_CTFFS + 1 ] ;
			memcpy( zeta_temp, zeta_array_AFROC[ i ][ j ], number_of_bins_AFROC[ i ][ j ] * sizeof( float ) ) ;
			zeta_temp[ number_of_bins_AFROC[ i ][ j ] ] = -UNINITIALIZED ;
			for( int t = 0 ; t < number_of_bins_AFROC[ i ][ j ] ; t++ ) {
				NL_BIN_AFROC[ i ][ j ][ t ] = array_count( NL[ i ][ j ], MAX_CASES[ 0 ] + MAX_CASES[ 1 ], MAX_NL, zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				LL_BIN_AFROC[ i ][ j ][ t ] = array_count( LL[ i ][ j ], MAX_CASES[ 1 ], MAX_LL, zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				FP_BIN_AFROC[ i ][ j ][ t ] = array_count( FP[ i ][ j ], MAX_CASES[ 0 ], zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
				TP_BIN_AFROC[ i ][ j ][ t ] = array_count( TP[ i ][ j ], MAX_CASES[ 1 ], zeta_temp[ t ], zeta_temp[ t + 1 ] ) ;
			}
		}
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int t = 0 ; t < number_of_bins_AFROC[ i ][ j ] ; t++ ) {
				NLF_BIN_AFROC[ i ][ j ][ t ] = 
					(float)total( &NL_BIN_AFROC[ i ][ j ][ number_of_bins_AFROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;
				LLF_BIN_AFROC[ i ][ j ][ t ] = 
					(float)total( &LL_BIN_AFROC[ i ][ j ][ number_of_bins_AFROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)( total( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
				FPF_BIN_AFROC[ i ][ j ][ t ] = 
					(float)total( &FP_BIN_AFROC[ i ][ j ][ number_of_bins_AFROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)MAX_CASES[ 0 ] ;
				TPF_BIN_AFROC[ i ][ j ][ t ] = 
					(float)total( &TP_BIN_AFROC[ i ][ j ][ number_of_bins_AFROC[ i ][ j ] - 1 - t ], t + 1 ) / (float)MAX_CASES[ 1 ] ;
			}
		}
	}
}
