#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <stdlib.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float ***AUC_array = NULL ;
float *bs_lb, *bs_ub ;



void alloc_bootstrap( void )
{
	if( AUC_array != NULL ) clear_bootstrap() ;

	AUC_array = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		AUC_array[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			AUC_array[ i ][ j ] = new float[ MAX_BS_COUNT ] ;
		}
	}
	bs_lb = new float[ N_TREATMENTS * N_TREATMENTS ] ;
	bs_ub = new float[ N_TREATMENTS * N_TREATMENTS ] ;
}


void clear_bootstrap( void )
{
	if( AUC_array == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] AUC_array[ i ][ j ] ;
		}
		delete[] AUC_array[ i ] ;
	}
	delete[] AUC_array ;
	delete[] bs_lb ;
	delete[] bs_ub ;

	AUC_array = NULL ;
}



void sort( float *source, float *target, int length )
{
	for( int i = 0 ; i < length ; i++ ) target[ i ] = source[ i ] ;
	for( int i = 0 ; i < length - 1 ; i++ ) {
		for( int j = i + 1 ; j < length ; j++ ) {
			if( target[ j ] < target[ i ] ) {
				float temp = target[ j ] ;
				target[ j ] = target[ i ] ;
				target[ i ] = temp ;
			}
		}
	}
}


void check_bounds( float *delta_Az, int len, float *lb, float *ub )
{
	float *sorted_delta_Az = new float[ len ] ;
	
	sort( delta_Az, sorted_delta_Az, len ) ;

	int lb_index = (int)( (float)( len ) * 0.025 ) - 1 ;
	int ub_index = (int)( (float)( len ) * 0.975 ) ;
	*lb = sorted_delta_Az[ lb_index ] ;
	*ub = sorted_delta_Az[ ub_index ] ;

	delete[] sorted_delta_Az ;
}



void bootstrap_FROC_Data( void )
{
	alloc_bootstrap() ;

	calculate_treatment_means() ;

	float *bs_diff = new float[ MAX_BS_COUNT ] ;

	int *nor_bs_index = new int[ MAX_CASES[ 0 ] ] ;
	int *abn_bs_index = new int[ MAX_CASES[ 1 ] ] ;
	int *reader_bs_index = new int[ MAX_READERS ] ;

	float **NL_bs = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
	float **LL_bs = new float *[ MAX_CASES[ 1 ] ] ;	
	int *N_Lesions_bs = new int[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ] ;

	for( int bs_index = 0 ; bs_index < MAX_BS_COUNT ; bs_index++ ) {
		for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) nor_bs_index[ k ] = rand() % MAX_CASES[ 0 ] ;
		for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) abn_bs_index[ k ] = rand() % MAX_CASES[ 1 ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) reader_bs_index[ j ] = rand() % MAX_READERS ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) NL_bs[ k ] = NL[ i ][ reader_bs_index[ j ] ][ nor_bs_index[ k ] ] ;
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) NL_bs[ k + MAX_CASES[ 0 ] ] = NL[ i ][ reader_bs_index[ j ] ][ abn_bs_index[ k ] + MAX_CASES[ 0 ] ] ;
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) LL_bs[ k ] = LL[ i ][ reader_bs_index[ j ] ][ abn_bs_index[ k ] ] ;
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) N_Lesions_bs[ k ] = N_Lesions_Per_Image[ abn_bs_index[ k ] ] ;
				float auc = MyFOM( NL_bs, LL_bs, N_Lesions_bs, MAX_CASES, MAX_NL, MAX_LL ) ;
				AUC_array[ i ][ j ][ bs_index ] = auc ;
			}
		}
	}

	int count = 0 ;
	for( int i1 = 0 ; i1 < N_TREATMENTS - 1 ; i1++ ) {
		for( int i2 = i1 + 1 ; i2 < N_TREATMENTS ; i2++ ) {
			for( int k = 0 ; k < MAX_BS_COUNT ; k++ ) {
				float auc1 = 0.0f, auc2 = 0.0f ;
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					auc1 += AUC_array[ i1 ][ j ][ k ] ;
					auc2 += AUC_array[ i2 ][ j ][ k ] ;
				}
				auc1 /= (float)MAX_READERS ;
				auc2 /= (float)MAX_READERS ;
				bs_diff[ k ] = auc1 - auc2 ;
			}
			check_bounds( bs_diff, MAX_BS_COUNT, &bs_lb[ count ], &bs_ub[ count ] ) ;
			count++ ;
		}
	}

	delete[] bs_diff ;

	delete[] nor_bs_index ;
	delete[] abn_bs_index ;
	delete[] reader_bs_index ;

	delete[] NL_bs ;
	delete[] LL_bs ;
	delete[] N_Lesions_bs ;
}
