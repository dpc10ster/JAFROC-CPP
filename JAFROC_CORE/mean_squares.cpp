#include "stdafx.h"
#include <float.h>
#include "JAFROC_CORE_DEF.h"


float MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC ;
float var_tr, var_tc, var_trc ;


float mean_pseudovalues_i( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int j, int k )
{
	double mean = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		mean += (double)PseudoValues[ i ][ j ][ k ] ;
	}
	mean /= (double)( N_TREATMENTS ) ;

	return (float)mean ;
}

float mean_pseudovalues_j( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int i, int k )
{
	double mean = 0.0f ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		mean += (double)PseudoValues[ i ][ j ][ k ] ;
	}
	mean /= (double)( MAX_READERS ) ;

	return (float)mean ;
}

float mean_pseudovalues_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int i, int j )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	double mean = 0.0f ;
	for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
		mean += (double)PseudoValues[ i ][ j ][ k ] ;
	}
	mean /= (double)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;

	return (float)mean ;
}

float mean_pseudovalues_j_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int i )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	double mean = 0.0f ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
			mean += (double)PseudoValues[ i ][ j ][ k ] ;
		}
	}
	mean /= (double)( MAX_READERS * ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ) ;

	return (float)mean ;
}

float mean_pseudovalues_i_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int j )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	double mean = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
			mean += (double)PseudoValues[ i ][ j ][ k ] ;
		}
	}
	mean /= (double)( N_TREATMENTS * ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ) ;

	return (float)mean ;
}

float mean_pseudovalues_i_j( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int k )
{
	double mean = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			mean += (double)PseudoValues[ i ][ j ][ k ] ;
		}
	}
	mean /= (double)( N_TREATMENTS * MAX_READERS ) ;

	return (float)mean ;
}

float mean_pseudovalues_i_j_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	double mean = 0.0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				mean += (double)PseudoValues[ i ][ j ][ k ] ;
			}
		}
	}
	mean /= (double)( N_TREATMENTS * MAX_READERS * ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ) ;

	return (float)mean ;
}

//void mean_squares( void )
extern "C" JAFROC_CORE_API void __cdecl mean_squares(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues,
	float *OUT_MS_T, float *OUT_MS_R, float *OUT_MS_C, float *OUT_MS_TC, float *OUT_MS_RC, float *OUT_MS_TR, float *OUT_MS_TRC,
	float *OUT_var_tr, float *OUT_var_tc, float *OUT_var_trc, int *err
	)
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	*err = 0 ;

	if( NCASES_ABN == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_ABN ;
		return ;
	}

	float mean_pseudovalues = mean_pseudovalues_i_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues ) ;
	int fpc = _fpclass( mean_pseudovalues ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	float t = (float)N_TREATMENTS ;
	float r = (float)MAX_READERS ;
	float c = (float)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;

	MS_T = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		MS_T += ( mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) - mean_pseudovalues ) * 
			    ( mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) - mean_pseudovalues ) ;
	}
	MS_T = MS_T * r * c / ( t - 1.0f ) ;
	fpc = _fpclass( MS_T ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	MS_R = 0.0f ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		MS_R += ( mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) - mean_pseudovalues ) * 
			    ( mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) - mean_pseudovalues ) ;
	}
	MS_R = MS_R * t * c / ( r - 1.0f ) ;
	fpc = _fpclass( MS_R ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	
	MS_C = 0.0 ;
	for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
		MS_C += ( mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) - mean_pseudovalues ) * 
			    ( mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) - mean_pseudovalues ) ;
	}
	MS_C = MS_C * t * r / ( c - 1.0f ) ;
	fpc = _fpclass( MS_C ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	MS_TC = 0.0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
			MS_TC += ( 
				mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) - 
				mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) + mean_pseudovalues ) *
				     ( 
				mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) - 
				mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) + mean_pseudovalues ) ;
		}
	}
	MS_TC = MS_TC * r / ( t - 1.0f ) / ( c - 1.0f ) ;
	fpc = _fpclass( MS_TC ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	MS_RC = 0.0 ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
			MS_RC += ( 
				mean_pseudovalues_i( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j, k ) - 
				mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) - 
				mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) + mean_pseudovalues ) *
					 ( 
				mean_pseudovalues_i( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j, k ) - 
				mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) - 
				mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) + mean_pseudovalues ) ;
		}
	}
	MS_RC = MS_RC * t / ( r - 1.0f ) / ( c - 1.0f ) ;
	fpc = _fpclass( MS_RC ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	MS_TR = 0.0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			MS_TR += ( 
				mean_pseudovalues_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, j ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) - 
				mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) + mean_pseudovalues ) *
					 ( 
				mean_pseudovalues_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, j ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) - 
				mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) + mean_pseudovalues ) ;
		}
	}
	MS_TR = MS_TR * c  / ( t - 1.0f ) / ( r - 1.0f ) ;
	fpc = _fpclass( MS_TR ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	MS_TRC = 0.0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				MS_TRC += 
					( PseudoValues[ i ][ j ][ k ] - mean_pseudovalues_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, j ) - 
					mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) - 
					mean_pseudovalues_i( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j, k )
					+ mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) + 
					mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) + 
					mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) - mean_pseudovalues ) * 
					( PseudoValues[ i ][ j ][ k ] - mean_pseudovalues_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, j ) - 
					mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) - 
					mean_pseudovalues_i( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j, k )
					+ mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) + 
					mean_pseudovalues_i_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, j ) +
					mean_pseudovalues_i_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, k ) - mean_pseudovalues ) ;
			}
		}
	}
	MS_TRC = MS_TRC / ( t - 1.0f ) / ( r - 1.0f ) / ( c  - 1.0f ) ;
	fpc = _fpclass( MS_TRC ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	var_tr = ( ( MS_TR - MS_TRC ) / c ) ;
	var_tc = ( ( MS_TC - MS_TRC ) / r ) ;
	var_trc = MS_TRC ;

	// return mean squares
	*OUT_MS_T = MS_T ;
	*OUT_MS_R = MS_R ;
	*OUT_MS_C = MS_C ;
	*OUT_MS_TC = MS_TC ;
	*OUT_MS_RC = MS_RC ;
	*OUT_MS_TR = MS_TR ;
	*OUT_MS_TRC = MS_TRC ;
	*OUT_var_tr = var_tr ;
	*OUT_var_tc = var_tc ;
	*OUT_var_trc = var_trc ;

}
