#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"
#include <float.h>
#include <math.h>
#include "nr.h"



// dpc!!! unsure about this code ; recheck with book which book??
// ORIGINAL CODE
void one_reader_anova( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues, 
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_CASES, int *err
					  )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	float SUM_PV = pow( total( PseudoValues, t, r, c ), 2.0f ) / (float)c / (float)t ;
	int fpc = _fpclass( SUM_PV ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	float SS_TRT = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		float temp = 0.0f ;	for( int k = 0 ; k < c ; k++ ) temp += PseudoValues[ i ][ 0 ][ k ] ;
		SS_TRT += pow( temp, 2.0f ) / (float)c ;
	}
	SS_TRT -= SUM_PV ;
	fpc = _fpclass( SS_TRT ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	float SS_B = 0.0f ;
	for( int k = 0 ; k < c ; k++ ) {
		float temp = 0.0f ;	for( int i = 0 ; i < t ; i++ ) temp += PseudoValues[ i ][ 0 ][ k ] ;
		SS_B += pow( temp, 2.0f ) / (float)t ;
	}
	SS_B -= SUM_PV ;
	fpc = _fpclass( SS_B ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	float SS_TOT = 0.0f ;
	for( int k = 0 ; k < c ; k++ ) {
		for( int i = 0 ; i < t ; i++ ) {
			SS_TOT += pow( PseudoValues[ i ][ 0 ][ k ], 2.0f ) ;
		}
	}
	SS_TOT -= SUM_PV ;
	fpc = _fpclass( SS_TOT ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	float SS_E = SS_TOT - SS_TRT - SS_B ;
	float F_TRT = SS_TRT / (float)( t - 1 ) ;
	F_TRT = F_TRT / ( SS_E / (float)( t - 1 ) / (float)( c - 1 ) ) ;
	float F_B = SS_B / (float)( c - 1 ) ;
	F_B = F_B / ( SS_E / (float)( t - 1 ) / (float)( c - 1 ) ) ;
	float p_TRT = 1.0f - f_pdf( F_TRT, (float)( t - 1 ), (float)( ( t - 1 ) * ( c - 1 ) ) ) ;
	float p_B = 1.0f - f_pdf( F_B, (float)( c - 1 ), (float)( ( t - 1 ) * ( c - 1 ) ) ) ;

	float MSE = SS_E / (float)( ( t - 1 ) * ( c - 1 ) ) ;
	fpc = _fpclass( MSE ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	float q = inv_studrange( (float)ALPHA, (float)( ( t - 1 ) * ( c - 1 ) ), (float)t ) ;
	float d = q / (float)sqrt( (double)c ) ;
	float CI_TRT = d * (float)sqrt( (double)MSE ) ;
	fpc = _fpclass( CI_TRT ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	Results[ 0 ] = F_TRT ;
	Results[ 1 ] = p_TRT ;
	fpc = _fpclass( F_TRT ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( p_TRT ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
/*
	if( Left != NULL ) {
		delete[] Left ;
		delete[] Right ;
	}
	Left = new float[ t * t ] ;
	Right = new float[ t * t ] ;
*/
	int inc = 0 ;
	for( int i1 = 0 ; i1 < t - 1 ; i1++ ) {
		for( int i2 = i1 + 1 ; i2 < t ; i2++ ) {
			float diff = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) ;
			float lower = diff - CI_TRT ;
			float upper = diff + CI_TRT ;
			fpc = _fpclass( lower ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}
			fpc = _fpclass( upper ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}
			Left[ inc ] = lower ;
			Right[ inc ] = upper ;
			inc++ ;
		}
	}
	DF[ 0 ] = (float)( t - 1 ) ;
	DF[ 1 ] = (float)( c - 1 ) ;
	fpc = _fpclass( DF[ 0 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( DF[ 1 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

/*
	if( TRT_CI_CASES != NULL ) {
		for( int i = 0 ; i < t ; i++ ) delete[] TRT_CI_CASES[ i ] ;
		delete[] TRT_CI_CASES ;
	}
	TRT_CI_CASES = new float *[ t ] ;
	for( int i = 0 ; i < t ; i++ ) {
		TRT_CI_CASES[ i ] = new float[ 2 ] ;
	}
*/
	float delta = t_cvf( ALPHA / 2.0f, (float)( c - 1 ) ) ;	// need this function

	for( int i = 0 ; i < t ; i++ ) {
		float std_err = stdev( PseudoValues[ i ][ 0 ], c ) / sqrt( (float)c ) ;
		TRT_CI_CASES[ i ][ 0 ] = mean( TR_MEANS[ i ], r ) - delta * std_err ;
		TRT_CI_CASES[ i ][ 1 ] = mean( TR_MEANS[ i ], r ) + delta * std_err ;
		fpc = _fpclass( TRT_CI_CASES[ i ][ 0 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
		fpc = _fpclass( TRT_CI_CASES[ i ][ 1 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
	}
}



void anova_for_each_trt_all( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float **TR_MEANS, float ***PseudoValues,
	float **TRT_CI_ALL, int *err
							)
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	float *SS_R = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) SS_R[ i ] = 0.0f ;

	for( int i = 0 ; i < t ; i++ ) {
		for( int j = 0 ; j < r ; j++ ) {
			SS_R[ i ] += 
				pow( ( 
				mean_pseudovalues_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, j ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) ), 2.0f ) ;
		}
		SS_R[ i ] = SS_R[ i ] * (float)c ;
	}
	float *MS_R = new float[ t ];
	for( int i = 0 ; i < t ; i++ ) MS_R[ i ] = SS_R[ i ] / (float)( r - 1 ) ;

	float *SS_C = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) SS_C[ i ] = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		for( int k = 0 ; k < c ; k++ ) {
			SS_C[ i ] += 
				pow( ( 
				mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ) ), 2.0f ) ;
		}
		SS_C[ i ] = SS_C[ i ] * (float)r ;
	}
	float *MS_C = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) MS_C[ i ] = SS_C[ i ] / (float)( c - 1 ) ;

	float *SS_RC = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) SS_RC[ i ] = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		for( int j = 0 ; j < r ; j++ ) {
			for( int k = 0 ; k < c ; k++ ) {
				SS_RC[ i ] += 
					pow( PseudoValues[ i ][ j ][ k ] - 
					mean_pseudovalues_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, j ) - 
					mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) + 
					mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ), 2.0f ) ;
			}
		}
	}
	float *MS_RC = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) MS_RC[ i ] = SS_RC[ i ] / (float)( ( r - 1 ) * ( c - 1 ) ) ;
/*
	if( TRT_CI_ALL != NULL ) {
		for( int i = 0 ; i < t ; i++ ) delete[] TRT_CI_ALL[ i ] ;
		delete[] TRT_CI_ALL ;
	}
	TRT_CI_ALL = new float *[ t ] ;
	for( int i = 0 ; i < t ; i++ ) TRT_CI_ALL[ i ] = new float[ 2 ] ;
*/
	for( int i = 0 ; i < t ; i++ ) {
		float temp = MS_C[ i ] - MS_RC[ i ] ; if( temp < 0.0f ) temp = 0.0f ;
		float var_rc = ( 1.0f / (float)r / (float)c ) * ( MS_R[ i ] + temp ) ;
		float temp1 = pow( ( MS_R[ i ] + temp ), 2.0f ) ;
		float temp2 = pow( MS_R[ i ], 2.0f ) / (float)( r - 1 ) + pow( MS_C[ i ], 2.0f ) / (float)( c - 1 ) +
			pow( MS_RC[ i ], 2.0f ) / (float)( ( r - 1 ) * ( c - 1 ) ) ;
		float df = temp1 / temp2 ;
		if( df < MIN_DDF ) df = MIN_DDF ;
		float delta = t_cvf( ALPHA / 2.0f, df ) ;

		float std_err = sqrt( var_rc ) ;

		TRT_CI_ALL[ i ][ 0 ] = mean( TR_MEANS[ i ], r ) - delta * std_err ;
		TRT_CI_ALL[ i ][ 1 ] = mean( TR_MEANS[ i ], r ) + delta * std_err ;

		int fpc = _fpclass( TRT_CI_ALL[ i ][ 0 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
		fpc = _fpclass( TRT_CI_ALL[ i ][ 1 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
	}
	delete[] SS_R ;
	delete[] MS_R ;
	delete[] SS_C ;
	delete[] MS_C ;
	delete[] SS_RC ;
	delete[] MS_RC ;
}

/*
void anova_readers_cases_each_modality( void )
{
	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	float *SS_R = new float[ t ] ; for( int i = 0 ; i < t ; i++ ) SS_R[ i ] = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		for( int j = 0 ; j < r ; j++ ) {
			SS_R[ i ] += pow( mean_pseudovalues_k( i, j ) - mean_pseudovalues_j_k( i ), 2.0f ) ;
		}
	}
	float *MS_R = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) MS_R[ i ] = SS_R[ i ] * (float)c / (float)( r - 1 ) ;

	float *SS_C = new float[ t ] ; for( int i = 0 ; i < t ; i++ ) SS_C[ i ] = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		for( int k = 0 ; k < c ; k++ ) {
			SS_C[ i ] += pow( mean_pseudovalues_j( i, k ) - mean_pseudovalues_j_k( i ), 2.0f ) ;
		}
	}
	float *MS_C = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) MS_C[ i ] = SS_C[ i ] * (float)r / (float)( c - 1 ) ;

	float *SS_RC = new float[ t ] ; for( int i = 0 ; i < t ; i++ ) SS_RC[ i ] = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		for( int j = 0 ; j < r ; j++ ) {
			for( int k = 0 ; k < c ; k++ ) {
				SS_RC[ i ] += pow(
					PseudoValues[ i ][ j ][ k ] - mean_pseudovalues_j( i, k ) - mean_pseudovalues_k( i, j ) + mean_pseudovalues_j_k( i )
					, 2.0f ) ;
			}
		}
	}
	float *MS_RC = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) MS_RC[ i ] = SS_RC[ i ] / (float)( ( r - 1 ) * ( c - 1 ) ) ;

	delete[] SS_R ;
	delete[] MS_R ;
	delete[] SS_C ;
	delete[] MS_C ;
	delete[] SS_RC ;
	delete[] MS_RC ;
}
*/

void anova_for_each_trt_cases( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float **TR_MEANS, float ***PseudoValues,
	float **TRT_CI_CASES, int *err
							  )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	float *SS_C = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) SS_C[ i ] = 0.0f ;
	for( int i = 0 ; i < t ; i++ ) {
		for( int k = 0 ; k < c ; k++ ) {
			SS_C[ i ] += pow( 
				mean_pseudovalues_j( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i, k ) - 
				mean_pseudovalues_j_k( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, PseudoValues, i ), 2.0 ) ;
		}
	}
	float *MS_C = new float[ t ] ;
	for( int i = 0 ; i < t ; i++ ) MS_C[ i ] = SS_C[ i ] / (float)( c - 1 ) ;
/*
	if( TRT_CI_CASES != NULL ) {
		for( int i = 0 ; i < t ; i++ ) delete[] TRT_CI_CASES[ i ] ;
		delete[] TRT_CI_CASES ;
	}
	TRT_CI_CASES = new float *[ t ] ;
	for( int i = 0 ; i < t ; i++ ) TRT_CI_CASES[ i ] = new float[ 2 ] ;
*/
	for( int i = 0 ; i < t ; i++ ) {
		float delta = t_cvf( ALPHA / 2.0f, (float)( c - 1 ) ) ;
		float std_err = sqrt( MS_C[ i ] / (float)c ) ;
		TRT_CI_CASES[ i ][ 0 ] = mean( TR_MEANS[ i ], MAX_READERS ) - delta * std_err ;
		TRT_CI_CASES[ i ][ 1 ] = mean( TR_MEANS[ i ], MAX_READERS ) + delta * std_err ;

		int fpc = _fpclass( TRT_CI_CASES[ i ][ 0 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
		fpc = _fpclass( TRT_CI_CASES[ i ][ 1 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
	}
	
	delete[] SS_C ;
	delete[] MS_C ;
}


void anova_for_each_trt_rdrs( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float **TR_MEANS, float ***PseudoValues,
	float **TRT_CI_RDRS, int *err
							 )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;
/*
	if( TRT_CI_RDRS != NULL ) {
		for( int i = 0 ; i < t ; i++ ) delete[] TRT_CI_RDRS[ i ] ;
		delete[] TRT_CI_RDRS ;
	}
	TRT_CI_RDRS = new float *[ t ] ;
	for( int i = 0 ; i < t ; i++ ) {
		TRT_CI_RDRS[ i ] = new float[ 2 ] ;
	}
*/	
	float delta = t_cvf( ALPHA / 2.0f, (float)( r - 1 ) ) ;

	for( int i = 0 ; i < t ; i++ ) {
		float std_err = stdev( TR_MEANS[ i ], MAX_READERS ) / sqrt( (float)r ) ;
		TRT_CI_RDRS[ i ][ 0 ] = mean( TR_MEANS[ i ], MAX_READERS ) - delta * std_err ;
		TRT_CI_RDRS[ i ][ 1 ] = mean( TR_MEANS[ i ], MAX_READERS ) + delta * std_err ;

		int fpc = _fpclass( TRT_CI_RDRS[ i ][ 0 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
		fpc = _fpclass( TRT_CI_RDRS[ i ][ 1 ] ) ;
		if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
			*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
		}
	}
}



void anova_random_all( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues,
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_ALL, int *err
					  )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	float temp ;

	//**** F **************
	float F ;
	float num = (float)MS_T ;
	temp = MS_TC - MS_TRC ;	if( temp < 0.0 ) temp = 0.0 ;
	float den = MS_TR + temp ;
	if( abs( den ) < MY_EPS ) {
		F = MAX_F ;
	} else {
		if( ( num / den ) < MAX_F ) F = num / den ;
		else F = MAX_F ;
	}

	//**** DDF_H **************
	float ddf_H ;
	num = (float)( ( MS_TR + MS_TC - MS_TRC ) * ( MS_TR + MS_TC - MS_TRC ) ) ;
	float num_den = (float)( MS_TR * MS_TR ) ;
	float den_den = (float)( ( N_TREATMENTS - 1 ) * ( MAX_READERS - 1 ) ) ;
	if( abs( num_den ) < MY_EPS ) {
		ddf_H = MAX_DDF ;
	} else { 
		temp = ( num / ( num_den / den_den ) ) ;
		if( temp > MAX_DDF ) temp = MAX_DDF ;
		if( temp < MIN_DDF ) temp = MIN_DDF ;
		ddf_H = temp ;
	}
	float p_value = (float)( 1.0 - f_pdf( (float)F, (float)( N_TREATMENTS - 1 ), (float)ddf_H ) ) ;

	Results[ 0 ] = (float)F ;
	Results[ 1 ] = (float)p_value ;

	int fpc = _fpclass( F ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( p_value ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	
	DF[ 0 ] = (float)( N_TREATMENTS - 1 ) ;
	DF[ 1 ] = (float)( ddf_H ) ;

	fpc = _fpclass( DF[ 0 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( DF[ 1 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	// *****  95% CI's for differences ********************************************

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	temp = MS_TC - MS_TRC ; if( temp < 0.0f ) temp = 0.0f ;
	float MS_DEN_DBM = MS_TR + temp ;
	float CI_DELTA = t_cvf( ALPHA / 2.0f, ddf_H ) * sqrt( 2.0f / (float)r / (float)c * MS_DEN_DBM ) ;
/*
	if( Left != NULL ) delete[] Left ;
	Left = new float[ t * ( t - 1 ) / 2 ] ;
	if( Right != NULL ) delete[] Right ;
	Right = new float[ t * ( t - 1 ) / 2 ] ;
*/
	int inc = 0 ;
	for( int i1 = 0 ; i1 < t - 1 ; i1++ ) {
		for( int i2 = i1 + 1 ; i2 < t ; i2++ ) {
			Left[ inc ] = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) - CI_DELTA ;
			Right[ inc ] = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) + CI_DELTA ;

			fpc = _fpclass( Left[ inc ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}
			fpc = _fpclass( Right[ inc ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}

			inc++ ;
		}
	}

	// *****  95% CI's for modalities ********************************************
	anova_for_each_trt_all( 
		N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues, TRT_CI_ALL, err
		) ;
}


void anova_random_cases( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues, 
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_CASES, int *err
						)
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;


	if( fabs( MS_TC ) < 1e-8 ) {
//		anova_error_exit() ;
	}


	// *****  calculate F-stat, p-value, and ddf **********************************
	float F_DBM = fabs( MS_T / MS_TC ) ;
	if( F_DBM > 1e3f ) F_DBM = 1e3f ;	// Eqn. 7 in MRMC_Sample_SizeProgram_User_Guide.pdf

	float ddf = (float)( ( t - 1 ) * ( c - 1 ) ) ;
	float p_value = 1.0f - f_pdf( F_DBM, (float)( t - 1 ), ddf ) ;

	Results[ 0 ] = F_DBM ;
	Results[ 1 ] = p_value ;

	int fpc = _fpclass( F_DBM ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( p_value ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	DF[ 0 ] = (float)( t - 1 ) ;
	DF[ 1 ] = ddf ;

	fpc = _fpclass( DF[ 0 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( DF[ 1 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	// *****  95% CI's for differences ********************************************
	float CI_DELTA = t_cvf( ALPHA / 2.0f, ddf ) * sqrt( 2.0f / (float)r / (float)c * MS_TC ) ;
/*
	if( Left != NULL ) delete[] Left ;
	Left = new float[ t * ( t - 1 ) / 2 ] ;
	if( Right != NULL ) delete[] Right ;
	Right = new float[ t * ( t - 1 ) / 2 ] ;
*/
	int inc = 0 ;
	for( int i1 = 0 ; i1 < t - 1 ; i1++ ) {
		for( int i2 = i1 + 1 ; i2 < t ; i2++ ) {
			Left[ inc ] = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) - CI_DELTA ;
			Right[ inc ] = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) + CI_DELTA ;

			fpc = _fpclass( Left[ inc ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}
			fpc = _fpclass( Right[ inc ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}

			inc++ ;
		}
	}

	//*****  95% CI's for modalities ********************************************
	anova_for_each_trt_cases(
		N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues, TRT_CI_CASES, err
		) ;
}


void anova_random_rdrs( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues,
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_RDRS, int *err
					   )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;
	
	if( fabs( MS_TR ) < 1e-8 ) {
//		anova_error_exit() ;
	}

	//*****  calculate F-stat, p-value, and ddf **********************************
	float F_DBM = fabs( MS_T / MS_TR ) ;
	if( F_DBM > 1e3f ) F_DBM = 1e3f ;	// Eqn. 10 in MRMC_Sample_SizeProgram_User_Guide.pdf  ??? dpc!!!

	int t = N_TREATMENTS ;
	int r = MAX_READERS ;
	int c = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	float ddf = (float)( ( t - 1 ) * ( r - 1 ) ) ;
	float p_value = 1.0f - f_pdf( F_DBM, (float)( t - 1 ), ddf ) ;

	Results[ 0 ] = F_DBM ;
	Results[ 1 ] = p_value ;

	int fpc = _fpclass( F_DBM ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( p_value ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	DF[ 0 ] = (float)( t - 1 ) ;
	DF[ 1 ] = ddf ;

	fpc = _fpclass( DF[ 0 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	fpc = _fpclass( DF[ 1 ] ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}

	//*****  95% CI's for differences ********************************************
	float CI_DELTA = t_cvf( ALPHA / 2.0f, ddf ) * pow( ( 2.0f / (float)r / (float)c * MS_TR ), 0.5f ) ;
/*
	if( Left != NULL ) delete[] Left ;
	Left = new float[ t * ( t - 1 ) / 2 ] ;
	if( Right != NULL ) delete[] Right ;
	Right = new float[ t * ( t - 1 ) / 2 ] ;
*/
	int inc = 0 ;
	for( int i1 = 0 ; i1 < t - 1 ; i1++ ) {
		for( int i2 = i1 + 1 ; i2 < t ; i2++ ) {
			Left[ inc ] = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) - CI_DELTA ;
			Right[ inc ] = mean( TR_MEANS[ i1 ], MAX_READERS ) - mean( TR_MEANS[ i2 ], MAX_READERS ) + CI_DELTA ;

			fpc = _fpclass( Left[ inc ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}
			fpc = _fpclass( Right[ inc ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}

			inc++ ;
		}
	}

	//*****  95% CI's for modalities ********************************************
	anova_for_each_trt_rdrs(
		N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues, TRT_CI_RDRS, err
		) ;
}



extern "C" JAFROC_CORE_API void __cdecl anova( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues, int anova_random,
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI, int *err
					   )
{
	*err = 0 ;

	if( NCASES_ABN == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_ABN ;
		return ;
	}

	switch( anova_random ) {
		case ANOVA_RANDOM_ALL:
			{
				if( MAX_READERS == 1 ) {
					*err |= JAFROC_DLL_ERROR_INVALID_ANOVA ;
					return ;
				} else {
					anova_random_all(
						N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues,
						MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
						Results, DF, Left, Right, TRT_CI, err
						) ;				
					return ;
				}
			}
			break ;
		case ANOVA_RANDOM_READERS:
			{
				if( MAX_READERS == 1 ) {
					*err |= JAFROC_DLL_ERROR_INVALID_ANOVA ;
					return ;
				} else {
					anova_random_rdrs(
						N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues,
						MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
						Results, DF, Left, Right, TRT_CI, err
						) ;				
					return ;
				}
			}
			break; 
		case ANOVA_RANDOM_CASES:
			{
				if( MAX_READERS == 1 ) {
					one_reader_anova(
						N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues,
						MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
						Results, DF, Left, Right, TRT_CI, err
						) ;				
					return ;
				} else {
					anova_random_cases(
						N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues,
						MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
						Results, DF, Left, Right, TRT_CI, err
						) ;				
					return ;
				}
			}
			break ;
		default:
			*err |= JAFROC_DLL_ERROR_INVALID_ANOVA ;
			return ;
			break ;
	}
}
