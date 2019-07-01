#include "stdafx.h"
#include <float.h>
#include <math.h>
#include "JAFROC_CORE_DEF.h"
#include "../JAFROC/common.h"


float TrapezoidalArea( float *noise, int n_noise, float *signal, int n_signal )
{
	double tw = 0.0 ;
	float ret ;

	for( int i = 0 ; i < n_noise ; i++ ) {
		for( int j = 0 ; j < n_signal ; j++ ) {
			if( noise[ i ] < signal[ j ] ) tw += 1.0 ;
			else if( noise[ i ] == signal[ j ] ) tw += 0.5 ;
		}
	}
	ret = (float)( tw / (double)( n_noise * n_signal ) ) ;

	return ret ;
}

float TrapezoidalAreaWeighted( float *noise, int n_noise, float *signal, int n_signal, float *weights )
{
	double tw = 0.0 ;
	float ret ;

	for( int i = 0 ; i < n_noise ; i++ ) {
		for( int j = 0 ; j < n_signal ; j++ ) {
			if( noise[ i ] < signal[ j ] ) tw += ( 1.0 ) * weights[ j ] ;
			else if( noise[ i ] == signal[ j ] ) tw += ( 0.5 ) * weights[ j ] ;
		}
	}
	ret = (float)( tw / (double)( n_noise * n_signal ) ) ;

	return ret ;
}

float comp_phi( float a, float b )
{
	float ret = 0 ;
	if( a < b ) ret = 1.0f ;
	else if( a == b ) ret = 0.5f ;

	return ret ;
}



// ROI method
float FOM_ROI( int ncases_nor, int ncases_abn, int max_nl, int max_ll, int *n_les, float **nl, float **ll )
{
	double tw = 0.0 ;
	int nn = 0, ns = 0 ;
	float ret ;

	for( int k1 = 0 ; k1 < ncases_nor ; k1++ ) {
		for( int l1 = 0 ; l1 < max_nl ; l1++ ) {
			float a = nl[ k1 ][ l1 ] ;
			if( a == UNINITIALIZED ) continue ;
			nn++ ;
			for( int k2 = 0 ; k2 < ncases_abn ; k2++ ) {
				for( int l2 = 0 ; l2 < n_les[ k2 ] ; l2++ ) {
					float b ;
					b = ll[ k2 ][ l2 ] ;
					if( a < b ) tw += 1.0 ;
					else if( a == b ) tw += 0.5 ;
				}
			}
		}
	}

	for( int k1 = 0 ; k1 < ncases_abn ; k1++ ) {
		for( int l1 = 0 ; l1 < max_nl ; l1++ ) {
			float a = nl[ k1 + ncases_nor ][ l1 ] ;
			if( a == UNINITIALIZED ) continue ;
			nn++ ;
			for( int k2 = 0 ; k2 < ncases_abn ; k2++ ) {
				for( int l2 = 0 ; l2 < n_les[ k2 ] ; l2++ ) {
					float b ;
					b = ll[ k2 ][ l2 ] ;
					if( a < b ) tw += 1.0 ;
					else if( a == b ) tw += 0.5 ;
				}
			}
		}
	}

	for( int k2 = 0 ; k2 < ncases_abn ; k2++ ) {
		ns += n_les[ k2 ] ;
	}

	ret = (float)( tw / (double)nn / (double)ns ) ;
	return ret ;
}


float ROC_FOM( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	float *tp = new float[ max_cases[ 1 ] ] ;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < max_ll ; l++ ) {
			if( ll[ k ][ l ] > max )
				max = ll[ k ][ l ] ;
		}
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k + max_cases[ 0 ] ][ l ] > max )
				max = nl[ k + max_cases[ 0 ] ][ l ] ;
		}
		tp[ k ] = max ;
	}

	float *fp = new float[ max_cases[ 0 ] ] ;
	for( int k = 0 ; k < max_cases[ 0 ] ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] > max )
				max = nl[ k ][ l ] ;
		}
		fp[ k ] = max ;
	}

	float ret = TrapezoidalArea( fp, max_cases[ 0 ], tp, max_cases[ 1 ] ) ;

	delete[] tp ;
	delete[] fp ;

	return ret ;
}


float HR_Sensitivity( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int tp_count = 0;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < n_lesions_per_image[k] ; l++ ) {
			if( ll[ k ][ l ] > max )
				max = ll[ k ][ l ] ;
		}
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k + max_cases[ 0 ] ][ l ] > max )
				max = nl[ k + max_cases[ 0 ] ][ l ] ;
		}
		if (max > UNINITIALIZED) tp_count++;
	}

	float ret = (tp_count+0.0)/max_cases[ 1 ]; 
	return  ret;
}



float HR_Specificity( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int fp_count = 0;
	for( int k = 0 ; k < max_cases[ 0 ] ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] > max )
				max = nl[ k ][ l ] ;
		}
		if (max > UNINITIALIZED) fp_count++;
	}

	float ret = (fp_count+0.0)/max_cases[ 0 ]; 

	return 1 - ret ;
}


float SONG_FOM_A1( int ncases_nor, int ncases_abn, int max_nl, int max_ll, int *n_les, float **nl, float **ll )
{
	float *x = new float[ ncases_nor ] ;
	float *y = new float[ ncases_abn ] ;
	int p_x_0 = 0 ;
	int p_y_0 = 0 ;

	double tw = 0.0 ;
	float ret ;

	for( int k = 0 ; k < ncases_nor ; k++ ) {
		float m = 0.0f ;
		int inc = 0 ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] != UNINITIALIZED ) {
				m += nl[ k ][ l ] ;
				inc++ ;
			}
		}
		if( inc == 0 ) {
			x[ k ] = UNINITIALIZED ;
			p_x_0++ ;
		} else {
			x[ k ] = m / (float)inc ;
		}
	}
	for( int k = 0 ; k < ncases_abn ; k++ ) {
		float m1 = 0.0f ;
		int inc = 0 ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k + ncases_nor ][ l ] != UNINITIALIZED ) {
				m1 += nl[ k + ncases_nor ][ l ] ;
				inc++ ;
			}
		}
		if( inc == 0 ) {
			m1 = UNINITIALIZED ;
		} else {
			m1 = m1 / (float) inc ;
		}
		float m2 = 0.0f ;
		inc = 0 ;
		for( int l = 0 ; l < n_les[ k ] ; l++ ) {
			if( ll[ k ][ l ] != UNINITIALIZED ) {
				m2 += ll[ k ][ l ] ;
				inc++ ;
			}
		}
		if( inc == 0 ) {
			m2 = UNINITIALIZED ;
		} else {
			m2 = m2 / (float) inc ;
		}

		float m ;
		if( m1 > m2 ) m = m1 ; else m = m2 ;
		y[ k ] = m ;
		if( m == UNINITIALIZED ) {
			p_y_0++ ;
		}
	}
	
	for( int k1 = 0 ; k1 < ncases_nor ; k1++ ) {
		for( int k2 = 0 ; k2 < ncases_abn ; k2++ ) {
			if( x[ k1 ] != UNINITIALIZED && y[ k2 ] != UNINITIALIZED ) {
				if( x[ k1 ] < y[ k2 ] ) tw += 1.0 ;
				else if( x[ k1 ] == y[ k2 ] ) tw += 0.5 ;
			}
		}
	}
	ret = (float)( tw / (double)ncases_nor / (double)ncases_abn ) + (float)( ( (double)p_x_0 / (double)ncases_nor ) * ( 1.0 - 0.5 * (double)p_y_0 / (double)ncases_abn ) ) ;

	delete[] x ;
	delete[] y ;
	return ret ;
}


float SONG_A1_Specificity( int ncases_nor, int ncases_abn, int max_nl, int max_ll, int *n_les, float **nl, float **ll )
{
	int fp_count = 0;
	for( int k = 0 ; k < ncases_nor ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] > max )
				max = nl[ k ][ l ] ;
		}
		if (max > UNINITIALIZED) fp_count++;
	}

	float ret = (fp_count+0.0)/ncases_nor; 
	return  1- ret;
}


float SONG_A1_Sensitivity( int ncases_nor, int ncases_abn, int max_nl, int max_ll, int *n_les, float **nl, float **ll )
{
	int tp_count = 0;
	for( int k = 0 ; k < ncases_abn ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < n_les[k] ; l++ ) {
			if( ll[ k ][ l ] > max )
				max = ll[ k ][ l ] ;
		}
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k + ncases_nor ][ l ] > max )
				max = nl[ k + ncases_nor ][ l ] ;
		}
		if (max > UNINITIALIZED) tp_count++;
	}

	float ret = (tp_count+0.0)/ncases_abn; 
	return  ret;
}


float SONG_FOM_A2( int ncases_nor, int ncases_abn, int max_nl, int max_ll, int *n_les, float **nl, float **ll )
{
	int p_x_0 = 0 ;
	int p_y_0 = 0 ;

	double tw = 0.0 ;
	float ret ;

	for( int k = 0 ; k < ncases_nor ; k++ ) {
		float m = maximum( nl[ k ], max_nl ) ;
		if( m == UNINITIALIZED ) {
			p_x_0++ ;
		}
	}
	for( int k = 0 ; k < ncases_abn ; k++ ) {
		float m1 = maximum( nl[ k + ncases_nor ], max_nl ) ;
		float m2 = maximum( ll[ k ], n_les[ k ] ) ;
		float m ;
		if( m1 > m2 ) m = m1 ; else m = m2 ;
		if( m == UNINITIALIZED ) {
			p_y_0++ ;
		}
	}

	for( int s0 = 0 ; s0 < ncases_nor ; s0++ ) {
		for( int s1 = 0 ; s1 < ncases_abn ; s1++ ) {
			int n0 = 0, n1 = 0, a = 0 ;
			for( int l = 0 ; l < max_nl ; l++ ) {
				if( nl[ s0 ][ l ] != UNINITIALIZED ) n0++ ;
			}
			for( int l = 0 ; l < max_nl ; l++ ) {
				if( nl[ s1 + ncases_nor ][ l ] != UNINITIALIZED ) n1++ ;
			}
			for( int l = 0 ; l < n_les[ s1 ] ; l++ ) {
				if( ll[ s1 ][ l ] != UNINITIALIZED ) a++ ;
			}
			if( n0 > 0 && ( n1 + a ) > 0  ) {
				float v1 = 0.5f ;
				float v2 = 0.0f ;
				for( int l0 = 0 ; l0 < max_nl ; l0++ ) {
					if( nl[ s0 ][ l0 ] != UNINITIALIZED ) {
						for( int l1 = 0 ; l1 < max_nl ; l1++ ) {
							if( nl[ s1 + ncases_nor ][ l1 ] != UNINITIALIZED ) {
								v2 += comp_phi( nl[ s0 ][ l0 ], nl[ s1 + ncases_nor ][ l1 ] ) ;
							}
						}
						for( int l2 = 0 ; l2 < n_les[ s1 ] ; l2++ ) {
							if( ll[ s1 ][ l2 ] != UNINITIALIZED ) {
								v2 += comp_phi( nl[ s0 ][ l0 ], ll[ s1 ][ l2 ] ) ;
							}
						}
					}
				}
				v2 = v2 / (float)n0 / (float)( n1 + a ) ;
				tw += comp_phi( v1, v2 ) ;
			}
		}
	}

	ret = (float)( tw / (double)ncases_nor / (double)ncases_abn ) + (float)( ( (double)p_x_0 / (double)ncases_nor ) * ( 1.0 - 0.5 * (double)p_y_0 / (double)ncases_abn ) ) ;

	return ret ;
}



float JAFROC1_FOM( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int nles_total = 0 ;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		nles_total += n_lesions_per_image[ k ] ;
	}



	float *les = new float[ nles_total ] ;
	int inc = 0 ;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		for( int l = 0 ; l < n_lesions_per_image[ k ] ; l++ ) {
			les[ inc++ ] = ll[ k ][ l ] ;
		}
	}

	float *fp = new float[ max_cases[ 0 ] + max_cases[ 1 ] ] ;
	for( int k = 0 ; k < max_cases[ 0 ] + max_cases[ 1 ] ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] > max )
				max = nl[ k ][ l ] ;
		}
		fp[ k ] = max ;
	}

	float ret = TrapezoidalArea( fp, max_cases[ 0 ] + max_cases[ 1 ], les, nles_total ) ;

	delete[] les ;
	delete[] fp ;

	return ret ;
}


float JAFROC_FOM_internal( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int nles_total = 0 ;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		nles_total += n_lesions_per_image[ k ] ;
	}

	float *les = new float[ nles_total ] ;
	int inc = 0 ;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		for( int l = 0 ; l < n_lesions_per_image[ k ] ; l++ ) {
			les[ inc++ ] = ll[ k ][ l ] ;
		}
	}

	float *fp = new float[ max_cases[ 0 ] ] ;
	for( int k = 0 ; k < max_cases[ 0 ] ; k++ ) {
		float max = UNINITIALIZED ;
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] > max )
				max = nl[ k ][ l ] ;
		}
		fp[ k ] = max ;
	}

	float ret = TrapezoidalArea( fp, max_cases[ 0 ], les, nles_total ) ;

	delete[] les ;
	delete[] fp ;

	return ret ;
}


float JAFROC1_FOM_Weighted( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll, float **weights )
{
	double ret = 0.0 ;

	for( int na = 0 ; na < max_cases[ 1 ] ; na++ ) {
		for( int nn = 0 ; nn < max_cases[ 0 ] + max_cases[ 1 ] ; nn++ ) {
			for( int nles = 0 ; nles < n_lesions_per_image[ na ] ; nles++ ) {
				float fp = UNINITIALIZED ;
				for( int abn_index = 0 ; abn_index < max_nl ; abn_index++ )
					if( nl[ nn ][ abn_index ] > fp ) fp = nl[ nn ][ abn_index ] ;
				ret += weights[ na ][ nles ] * comp_phi( fp, ll[ na ][ nles ] ) ;
			}
		}
	}
	ret /= (double)( max_cases[ 0 ] + max_cases[ 1 ] ) * (double)max_cases[ 1 ] ;

	return (float)ret ;
}


float JAFROC_FOM_Weighted( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll, float **weights )
{
	double  ret = 0.0 ;

	for( int na = 0 ; na < max_cases[ 1 ] ; na++ ) {
		for( int nn = 0 ; nn < max_cases[ 0 ] ; nn++ ) {
			for( int nles = 0 ; nles < n_lesions_per_image[ na ] ; nles++ ) {
				float fp = UNINITIALIZED ;
				for( int abn_index = 0 ; abn_index < max_nl ; abn_index++ )
					if( nl[ nn ][ abn_index ] > fp ) fp = nl[ nn ][ abn_index ] ;
				ret += weights[ na ][ nles ] * comp_phi( fp, ll[ na ][ nles ] ) ;
			}
		}
	}
	ret /= (double)max_cases[ 0 ] * (double)max_cases[ 1 ] ;

	return (float)ret ;
}



float MaxLLF( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int TotalNumOfMarks = 0;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		for( int l = 0 ; l < n_lesions_per_image [ k ] ; l++ ) {
			if( ll[ k ][ l ] != UNINITIALIZED )
				TotalNumOfMarks++;
		}
	}

	int nles_total = 0 ;
	for( int k = 0 ; k < max_cases[ 1 ] ; k++ ) {
		nles_total += n_lesions_per_image[ k ] ;
	}

	float ret = (TotalNumOfMarks+0.0)/nles_total ;

	return ret ;
}



float MaxNLF( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int TotalNumOfMarks = 0;
	for( int k = 0 ; k < max_cases[ 0 ] ; k++ ) {
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] != UNINITIALIZED )
				TotalNumOfMarks++;
		}
	}

	float ret = (TotalNumOfMarks+0.0)/max_cases[ 0 ] ;

	return ret ;
}


float MaxNLF_AllCases( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
int TotalNumOfMarks = 0;
for( int k = 0 ; k < max_cases[ 0 ] + max_cases[ 1 ] ; k++ ) {
for( int l = 0 ; l < max_nl ; l++ ) {
if( nl[ k ][ l ] != UNINITIALIZED )
TotalNumOfMarks++;
}
}

float ret = (TotalNumOfMarks+0.0)/ (max_cases[ 0 ] + max_cases[ 1 ]) ;

return ret ;
}



float ExpTransformBasedSpecificity( float **nl, float **ll, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll )
{
	int TotalNumOfMarks = 0;
	for( int k = 0 ; k < max_cases[ 0 ] ; k++ ) {
		for( int l = 0 ; l < max_nl ; l++ ) {
			if( nl[ k ][ l ] != UNINITIALIZED )
				TotalNumOfMarks++;
		}
	}

	float ret = (TotalNumOfMarks+0.0)/max_cases[ 0 ] ;

	ret = exp(-ret);

	return ret ;
}





float MyFOM( float **NL_ij, float **LL_ij, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll, float **weights, int analysis_method )
{
	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
			return JAFROC1_FOM( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC:
			return JAFROC_FOM_internal( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_HR_ROC:
			return ROC_FOM( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
			return JAFROC1_FOM_Weighted( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll, weights ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC_WEIGHTED:
			return JAFROC_FOM_Weighted( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll, weights ) ;
			break ;
		case ANALYSIS_METHOD_HR_SENSITIVITY:
			return HR_Sensitivity( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_HR_SPECIFICITY:
			return HR_Specificity( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll) ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A1:
			return SONG_FOM_A1( max_cases[ 0 ], max_cases[ 1 ], max_nl, max_ll, n_lesions_per_image, NL_ij, LL_ij ) ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A2:
			return SONG_FOM_A2( max_cases[ 0 ], max_cases[ 1 ], max_nl, max_ll, n_lesions_per_image, NL_ij, LL_ij ) ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY:
			return SONG_A1_Specificity( max_cases[ 0 ], max_cases[ 1 ], max_nl, max_ll, n_lesions_per_image, NL_ij, LL_ij ) ;
			break;
		case ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY:
			return SONG_A1_Sensitivity( max_cases[ 0 ], max_cases[ 1 ], max_nl, max_ll, n_lesions_per_image, NL_ij, LL_ij ) ;
			break;
		case ANALYSIS_METHOD_ROI:
			return FOM_ROI( max_cases[ 0 ], max_cases[ 1 ], max_nl, max_ll, n_lesions_per_image, NL_ij, LL_ij ) ;
			break ;
		case ANALYSIS_METHOD_MAX_NLF:
			return MaxNLF( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_MAX_LLF:
			return MaxLLF( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
			return ExpTransformBasedSpecificity( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ) ;
			break ;
		case ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS:
			return MaxNLF_AllCases( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll );
			//return MaxLLF( NL_ij, LL_ij, n_lesions_per_image, max_cases, max_nl, max_ll ); // to investigate effect of including normal cases that have no effect on FOM
			break ;
	}
	return -1.0f ;
}



extern "C" JAFROC_CORE_API float __cdecl JAFROC_FOM( 
	int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float **NL, float **LL, int *N_Lesions_Per_Image, float **weights,
	int analysis_method, int *err )
{
	float ret ;
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	*err = 0 ;

	if( NCASES_ABN == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_ABN ;
	}
	if( analysis_method != ANALYSIS_METHOD_JAFROC1 && NCASES_NOR == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_NOR ;
	}
	if( MAX_NL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_NL ;
	}
	if( MAX_LL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_LL ;
	}
	if( *err != 0 ) {
		return -1.0f ;
	}

	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
			ret = JAFROC1_FOM( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			return ret ;
			break ;
		case ANALYSIS_METHOD_JAFROC:
			ret = JAFROC_FOM_internal( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			return ret ;
			break ;
		case ANALYSIS_METHOD_HR_ROC:
			ret = ROC_FOM( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			return ret ;
			break ;
		case ANALYSIS_METHOD_HR_SENSITIVITY:
			ret = HR_Sensitivity( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			break ;
		case ANALYSIS_METHOD_HR_SPECIFICITY:
			ret = HR_Specificity( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY:
			ret = SONG_A1_Specificity( MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, N_Lesions_Per_Image, NL, LL ) ;
			break;
		case ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY:
			ret = SONG_A1_Sensitivity( MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, N_Lesions_Per_Image, NL, LL ) ;
			break;
		case ANALYSIS_METHOD_SONG_FOM_A1:
			ret = SONG_FOM_A1( MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, N_Lesions_Per_Image, NL, LL ) ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A2:
			ret = SONG_FOM_A2( MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, N_Lesions_Per_Image, NL, LL ) ;
			break ;
		case ANALYSIS_METHOD_ROI:
			ret = FOM_ROI( MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, N_Lesions_Per_Image, NL, LL ) ;
			break ;
		case ANALYSIS_METHOD_MAX_NLF:
			ret =  MaxNLF( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			break ;
		case ANALYSIS_METHOD_MAX_LLF:
			ret =  MaxLLF( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			break ;
		case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
			ret =  ExpTransformBasedSpecificity( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			break ;
		case ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS:
			//ret =  MaxNLF_AllCases( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			ret =  MaxLLF( NL, LL, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL ) ;
			break;
		default:
			*err |= JAFROC_DLL_ERROR_INVALID_METHOD ;
			ret = -1.0f ;
			break ;
	}

	int fpc = _fpclass( ret ) ;
	if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
		*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
	}
	return ret ;
}
