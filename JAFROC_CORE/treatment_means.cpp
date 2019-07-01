#include "stdafx.h"
#include <float.h>
#include "JAFROC_CORE_DEF.h"



bool calculate_treatment_means(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Image, float **weights, int analysis_method, 
	float **TR_MEANS, int *err )
{
	float **NL_ij, **LL_ij ;

	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			NL_ij = NL[ i ][ j ] ;
			LL_ij = LL[ i ][ j ] ;
			TR_MEANS[ i ][ j ] = MyFOM( NL_ij, LL_ij, N_Lesions_Per_Image, MAX_CASES, MAX_NL, MAX_LL, weights, analysis_method ) ;

			int fpc = _fpclass( TR_MEANS[ i ][ j ] ) ;
			if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
				*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
			}
		}
	}

	return true ;
}