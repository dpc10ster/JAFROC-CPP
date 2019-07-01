#include "stdafx.h"
#include "../common.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float ***zeta_array_ROC = NULL ;
int **number_of_bins_ROC ;
float ***zeta_array_AFROC = NULL ;
int **number_of_bins_AFROC ;
float ***zeta_array_FROC = NULL ;
int **number_of_bins_FROC ;



void common_cutoff_routine( float *bad, int bad_len, float *good, int good_len, float *zeta, int *zeta_len )
{
	int *c1 = new int[ bad_len ] ;
	float *bad1 = new float[ bad_len ] ;
	int bad1_len = 0 ;
	int *c2 = new int[ good_len ] ;
	float *good1 = new float[ good_len ] ;
	int good1_len = 0 ;

	where( bad, bad_len, "ne", UNINITIALIZED, c1, bad1, &bad1_len ) ;
	where( good, good_len, "ne", UNINITIALIZED, c2, good1, &good1_len ) ;

	if( bad1_len == 0 || good1_len == 0 ) {
		zeta[ 0 ] = UNINITIALIZED ;
		*zeta_len = 1 ;
		return ;
	}

	float min1 = minimum( bad1, bad1_len ) ;
	float min2 = minimum( good1, good1_len ) ;
	float max1 = maximum( bad1, bad1_len ) ;
	float max2 = maximum( good1, good1_len ) ;

	float cutoff1, cutoff2, cutoffmax, cutoff1_save ;
	float lowest_cutoff = UNINITIALIZED ;
	int min_bin_count = MIN_BIN_COUNT ;

	if( min1 < min2 ) cutoff1 = min1 ; else cutoff1 = min2 ; 
	cutoff1 -= 0.1f ; if( cutoff1 < lowest_cutoff ) cutoff1 = lowest_cutoff ;
	if( max1 > max2 ) cutoffmax = max1 ; else cutoffmax = max2 ; cutoffmax += 0.1f ;
	cutoff1_save = cutoff1 ;

again:
	cutoff1 = cutoff1_save ;
	cutoff2 = cutoff1_save ;

	int zeta_count = 0 ;
	zeta[ zeta_count++ ] = cutoff1_save ;

	float ctffdelta_org = ( cutoffmax - cutoff1 ) / 1000.0f ;
	float ctffdelta = ctffdelta_org ;

	float tempcutoff = cutoff1 ;

	while( 1 ) {
		if( cutoff1 >= cutoffmax ) break ;
		int badcount = 0 ;
		int goodcount = 0 ;

		for( int i = 0 ; i < bad1_len ; i++ ) {
			if( bad1[ i ] > cutoff2 && bad1[ i ] <= cutoff1 ) badcount++ ;
		}
		for( int i = 0 ; i < good1_len ; i++ ) {
			if( good1[ i ] > cutoff2 && good1[ i ] <= cutoff1 ) goodcount++ ;
		}

		if( badcount >= min_bin_count && goodcount >= min_bin_count ) {
			if( badcount > min_bin_count && goodcount > min_bin_count ) {	// if both nor and abn are exceeding this value then bin is too wide
				if( ctffdelta > 0.0f ) ctffdelta = -ctffdelta / 2.0f ;
				cutoff1 = cutoff1 + ctffdelta ;
				if( fabs( ctffdelta ) < fabs( ctffdelta_org ) / 256.0f ) {
					cutoff1 = cutoff1 - ctffdelta ;
					zeta[ zeta_count++ ] = cutoff1 ;
					ctffdelta = ctffdelta_org ;
					cutoff2 = cutoff1 ;
					tempcutoff = cutoff1 ;
				}
			} else {
				zeta[ zeta_count++ ] = cutoff1 ;
				ctffdelta = ctffdelta_org ;
				cutoff2 = cutoff1 ;
				tempcutoff = cutoff1 ;
			}
			if( zeta_count == MAX_CTFFS ) {
				min_bin_count++ ;
				goto again ;
			}
		} else {
			if( ctffdelta < 0.0 ) ctffdelta = -ctffdelta / 2.0f ;
			cutoff1 = cutoff1 + ctffdelta ;
		}
	}

	// discard last cutoff because its bin count should be less than 'min_bin_count'
	if( zeta_count > 1 )
		*zeta_len = zeta_count - 1 ;
	else
		*zeta_len = zeta_count ;

	// clear memory
	delete[] c1 ;
	delete[] bad1 ;
	delete[] c2 ;
	delete[] good1 ;
}


void init_zeta_roc( void )
{
	clear_zeta_roc() ;

	zeta_array_ROC = new float **[ N_TREATMENTS ] ;
	number_of_bins_ROC = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		zeta_array_ROC[ i ] = new float *[ MAX_READERS ] ;
		number_of_bins_ROC[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			zeta_array_ROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			number_of_bins_ROC[ i ][ j ] = 0 ;
		}
	}
}

void clear_zeta_roc( void )
{
	if( zeta_array_ROC != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				delete[] zeta_array_ROC[ i ][ j ] ;
			}
			delete[] zeta_array_ROC[ i ] ;
			delete[] number_of_bins_ROC[ i ] ;
		}
		delete[] zeta_array_ROC ;
		delete[] number_of_bins_ROC ;
		zeta_array_ROC = NULL ;
	}
}


void find_optimal_cutoffs_roc( void )
{
	// check for no normal cases!!!dpc!!!

	init_zeta_roc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int *c1 = new int[ MAX_CASES[ 0 ] ] ;
			int *c2 = new int[ MAX_CASES[ 1 ] ] ;
			float *FP_ij_flat = new float[ MAX_CASES[ 0 ] ] ;
			float *TP_ij_flat = new float[ MAX_CASES[ 1 ] ] ;
			int FP_ij_len ;
			int TP_ij_len ;

			where( FP[ i ][ j ], MAX_CASES[ 0 ], "ne", UNINITIALIZED, c1, FP_ij_flat, &FP_ij_len ) ;
			where( TP[ i ][ j ], MAX_CASES[ 1 ], "ne", UNINITIALIZED, c2, TP_ij_flat, &TP_ij_len ) ;

			common_cutoff_routine( FP_ij_flat, FP_ij_len, TP_ij_flat, TP_ij_len, zeta_array_ROC[ i ][ j ], &number_of_bins_ROC[ i ][ j ] ) ;

			delete[] c1 ;
			delete[] c2 ;
			delete[] FP_ij_flat ;
			delete[] TP_ij_flat ;
		}
	}
}


void init_zeta_afroc( void )
{
	clear_zeta_afroc() ;

	zeta_array_AFROC = new float **[ N_TREATMENTS ] ;
	number_of_bins_AFROC = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		zeta_array_AFROC[ i ] = new float *[ MAX_READERS ] ;
		number_of_bins_AFROC[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			zeta_array_AFROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			number_of_bins_AFROC[ i ][ j ] = 0 ;
		}
	}
}

void clear_zeta_afroc( void )
{
	if( zeta_array_AFROC != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				delete[] zeta_array_AFROC[ i ][ j ] ;
			}
			delete[] zeta_array_AFROC[ i ] ;
			delete[] number_of_bins_AFROC[ i ] ;
		}
		delete[] zeta_array_AFROC ;
		delete[] number_of_bins_AFROC ;
		zeta_array_AFROC = NULL ;
	}
}


void find_optimal_cutoffs_afroc( void )
{
	init_zeta_afroc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int *c1 = new int[ MAX_CASES[ 0 ] ] ;
			int *c2 = new int[ MAX_CASES[ 1 ] * MAX_LL ] ;
			float *FP_ij_flat = new float[ MAX_CASES[ 0 ] ] ;
			float *LL_ij_flat = new float[ MAX_CASES[ 1 ] * MAX_LL ] ;
			int FP_ij_len ;
			int LL_ij_len ;

			where( FP[ i ][ j ], MAX_CASES[ 0 ], "ne", UNINITIALIZED, c1, FP_ij_flat, &FP_ij_len ) ;
			where( LL[ i ][ j ], MAX_CASES[ 1 ], MAX_LL, "ne", UNINITIALIZED, c2, LL_ij_flat, &LL_ij_len ) ;

			common_cutoff_routine( FP_ij_flat, FP_ij_len, LL_ij_flat, LL_ij_len, zeta_array_AFROC[ i ][ j ], &number_of_bins_AFROC[ i ][ j ] ) ;

			delete[] c1 ;
			delete[] c2 ;
			delete[] FP_ij_flat ;
			delete[] LL_ij_flat ;
		}
	}
}



void init_zeta_froc( void )
{
	clear_zeta_froc() ;

	zeta_array_FROC = new float **[ N_TREATMENTS ] ;
	number_of_bins_FROC = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		zeta_array_FROC[ i ] = new float *[ MAX_READERS ] ;
		number_of_bins_FROC[ i ] = new int[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			zeta_array_FROC[ i ][ j ] = new float[ MAX_CTFFS ] ;
			number_of_bins_FROC[ i ][ j ] = 0 ;
		}
	}
}

void clear_zeta_froc( void )
{
	if( zeta_array_FROC != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				delete[] zeta_array_FROC[ i ][ j ] ;
			}
			delete[] zeta_array_FROC[ i ] ;
			delete[] number_of_bins_FROC[ i ] ;
		}
		delete[] zeta_array_FROC ;
		delete[] number_of_bins_FROC ;
		zeta_array_FROC = NULL ;
	}
}


void find_optimal_cutoffs_froc( void )
{
	// check for no normal cases!!!dpc!!!

	init_zeta_froc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int *c1 = new int[ MAX_CASES[ 0 ] * MAX_NL ] ;
			int *c2 = new int[ MAX_CASES[ 1 ] * MAX_LL ] ;
			float *NL_ij_flat = new float[ MAX_CASES[ 0 ] * MAX_NL ] ;
			float *LL_ij_flat = new float[ MAX_CASES[ 1 ] * MAX_LL ] ;
			int NL_ij_len ;
			int LL_ij_len ;

			where( NL[ i ][ j ], MAX_CASES[ 0 ], MAX_NL, "ne", UNINITIALIZED, c1, NL_ij_flat, &NL_ij_len ) ;
			where( LL[ i ][ j ], MAX_CASES[ 1 ], MAX_LL, "ne", UNINITIALIZED, c2, LL_ij_flat, &LL_ij_len ) ;

			common_cutoff_routine( NL_ij_flat, NL_ij_len, LL_ij_flat, LL_ij_len, zeta_array_FROC[ i ][ j ], &number_of_bins_FROC[ i ][ j ] ) ;

			delete[] c1 ;
			delete[] c2 ;
			delete[] NL_ij_flat ;
			delete[] LL_ij_flat ;
		}
	}
}
