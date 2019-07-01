#include "stdafx.h"
#include <float.h>
#include "JAFROC_CORE_DEF.h"
#include <math.h>
#include "../JAFROC/common.h"



float ***FOM_ARR = NULL ;

void clear_fom_arr( int N_TREATMENTS, int MAX_READERS )
{
	if( FOM_ARR != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				delete[] FOM_ARR[ i ][ j ] ;
			}
			delete[] FOM_ARR[ i ] ;
		}
		delete[] FOM_ARR ;
		FOM_ARR = NULL ;
	}
}

void alloc_fom_arr( int N_TREATMENTS, int MAX_READERS, int *MAX_CASES )
{
	clear_fom_arr( N_TREATMENTS, MAX_READERS ) ;

	FOM_ARR = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		FOM_ARR[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			FOM_ARR[ i ][ j ] = new float[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
		}
	}
}


extern "C" JAFROC_CORE_API void __cdecl jackknife_FROC_Data( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Image, 
	int analysis_method, float **weights,
	float **TR_MEANS, float ***PseudoValues, int *err )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	*err = 0 ;

	if( NCASES_ABN == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_ABN ;
	}

	if( ( analysis_method != ANALYSIS_METHOD_JAFROC1 && analysis_method != ANALYSIS_METHOD_JAFROC1_WEIGHTED ) && NCASES_NOR == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_NOR ;
	}

	if( MAX_NL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_NL ;
	}
	if( MAX_LL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_LL ;
	}
	if( N_TREATMENTS == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_TREATMENT ;
	}
	if( MAX_READERS == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_READER ;
	}

	if( *err != 0 ) {
		return ;
	}

	calculate_treatment_means( 
		N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Image, weights, analysis_method,
		TR_MEANS, err ) ;
	alloc_fom_arr( N_TREATMENTS, MAX_READERS, MAX_CASES ) ;

	float **NL_jk, **LL_jk ;
	float **weights_jk ;
	int *N_Lesions_jk ;
	int MAX_CASES_jk[ 2 ] ;

	NL_jk = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ] ;
	LL_jk = new float *[ MAX_CASES[ 1 ] ] ;
	weights_jk = new float *[ MAX_CASES[ 1 ] ] ;
	N_Lesions_jk = new int[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ] ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int jk = 0 ; jk < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; jk++ ) {
				// build a jackknifed NL array
				for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					if( k < jk ) {
						// doesn't copy actual data, just assigning pointer of the original data
						NL_jk[ k ] = NL[ i ][ j ][ k ] ;
					} else if( k > jk ) {
						NL_jk[ k - 1 ] = NL[ i ][ j ][ k ] ;
					}
				}
				// jackknifed LL array
				if( jk >= MAX_CASES[ 0 ] ) {
					for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
						if( k < jk ) {
							LL_jk[ k - MAX_CASES[ 0 ] ] = LL[ i ][ j ][ k - MAX_CASES[ 0 ] ] ;
							weights_jk[ k - MAX_CASES[ 0 ] ] = weights[ k - MAX_CASES[ 0 ] ] ;
							N_Lesions_jk[ k - MAX_CASES[ 0 ] ] = N_Lesions_Per_Image[ k - MAX_CASES[ 0 ] ] ;
						} else if( k > jk ) {
							LL_jk[ k - MAX_CASES[ 0 ] - 1 ] = LL[ i ][ j ][ k - MAX_CASES[ 0 ] ] ;
							weights_jk[ k - MAX_CASES[ 0 ] - 1 ] = weights[ k - MAX_CASES[ 0 ] ] ;
							N_Lesions_jk[ k - MAX_CASES[ 0 ] - 1 ] = N_Lesions_Per_Image[ k - MAX_CASES[ 0 ] ] ;
						}
					}
				} else {
					for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
						LL_jk[ k - MAX_CASES[ 0 ] ] = LL[ i ][ j ][ k - MAX_CASES[ 0 ] ] ;
						weights_jk[ k - MAX_CASES[ 0 ] ] = weights[ k - MAX_CASES[ 0 ] ] ;
						N_Lesions_jk[ k - MAX_CASES[ 0 ] ] = N_Lesions_Per_Image[ k - MAX_CASES[ 0 ] ] ;
					}
				}
				// MAX_CASES_jk
				if( jk >= MAX_CASES[ 0 ] ) {
					MAX_CASES_jk[ 0 ] = MAX_CASES[ 0 ] ;
					MAX_CASES_jk[ 1 ] = MAX_CASES[ 1 ] - 1 ;
				} else {
					MAX_CASES_jk[ 0 ] = MAX_CASES[ 0 ] - 1 ;
					MAX_CASES_jk[ 1 ] = MAX_CASES[ 1 ] ;
				}

				// calculate FOM
				FOM_ARR[ i ][ j ][ jk ] = MyFOM( NL_jk, LL_jk, N_Lesions_jk, MAX_CASES_jk, MAX_NL, MAX_LL, weights_jk, analysis_method ) ;

				int fpc = _fpclass( FOM_ARR[ i ][ j ][ jk ] ) ;
				if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
					*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
				}

				// calculate pseudo values
				PseudoValues[ i ][ j ][ jk ] = (float)( 
					(double)( TR_MEANS[ i ][ j ] ) * (double)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) -
					(double)( FOM_ARR[ i ][ j ][ jk ] ) * (double)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ) ) ;
			}
		}
	}

	// normalized pseudo values
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			double Yij = 0.0 ;
			for( int jk = 0 ; jk < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; jk++ ) {
				Yij += PseudoValues[ i ][ j ][ jk ] ;
			}
			Yij /= (double)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;
			for( int jk = 0 ; jk < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; jk++ ) {
				PseudoValues[ i ][ j ][ jk ] = PseudoValues[ i ][ j ][ jk ] + (float)( TR_MEANS[ i ][ j ] - Yij ) ;
			}
		}
	}
/*
	if( MAX_READERS > 1 ) {
		mean_squares() ;
	}
*/
	clear_fom_arr( N_TREATMENTS, MAX_READERS ) ;
	delete[] NL_jk ;
	delete[] LL_jk ;
	delete[] N_Lesions_jk ;
	delete[] weights_jk ;

	return ;
}


extern "C" JAFROC_CORE_API void __cdecl jackknife_FROC_Data_Normals( //dpc 5/1/13 vortex
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Image,
	int analysis_method,  float **weights,
	float **TR_MEANS, float ***PseudoValues, int *err )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	*err = 0 ;

	if( NCASES_ABN == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_ABN ;
	}
	if( ( analysis_method != ANALYSIS_METHOD_JAFROC1 && analysis_method != ANALYSIS_METHOD_JAFROC1_WEIGHTED ) && NCASES_NOR == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_NOR ;
	}

	if( MAX_NL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_NL ;
	}
	if( MAX_LL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_LL ;
	}
	if( N_TREATMENTS == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_TREATMENT ;
	}
	if( MAX_READERS == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_READER ;
	}

	if( *err != 0 ) {
		return ;
	}

	calculate_treatment_means( 
		N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Image, weights, analysis_method,
		TR_MEANS, err ) ;
	alloc_fom_arr( N_TREATMENTS, MAX_READERS, MAX_CASES ) ;

	float **NL_jk, **LL_jk ;
	float **weights_jk ;
	int *N_Lesions_jk ;
	int MAX_CASES_jk[ 2 ] ;

	NL_jk = new float *[ MAX_CASES[ 0 ] - 1 ] ;//dpc 5/1/13 vortex
	LL_jk = new float *[ MAX_CASES[ 1 ] ] ;
	weights_jk = new float *[ MAX_CASES[ 1 ] ] ;
	N_Lesions_jk = new int[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ] ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int jk = 0 ; jk < MAX_CASES[ 0 ] ; jk++ ) {//dpc 5/1/13 vortex
				// build a jackknifed NL array
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {//dpc 5/1/13 vortex
					if( k < jk ) {
						NL_jk[ k ] = NL[ i ][ j ][ k ] ;
					} else if( k > jk ) {
						NL_jk[ k - 1 ] = NL[ i ][ j ][ k ] ;
					}
				}
				// MAX_CASES_jk
				MAX_CASES_jk[ 0 ] = MAX_CASES[ 0 ] - 1 ;
				MAX_CASES_jk[ 1 ] = MAX_CASES[ 1 ] ;

				// calculate FOM
				FOM_ARR[ i ][ j ][ jk ] = MyFOM( NL_jk, LL_jk, N_Lesions_jk, MAX_CASES_jk, MAX_NL, MAX_LL, weights_jk, analysis_method ) ;

				int fpc = _fpclass( FOM_ARR[ i ][ j ][ jk ] ) ;
				if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
					*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
				}

				// calculate pseudo values
				PseudoValues[ i ][ j ][ jk ] = (float)( 
					(double)( TR_MEANS[ i ][ j ] ) * (double)( MAX_CASES[ 0 ] ) -
					(double)( FOM_ARR[ i ][ j ][ jk ] ) * (double)( MAX_CASES[ 0 ] - 1 ) ) ;
			}
		}
	}

	// normalized pseudo values
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			double Yij = 0.0 ;
			for( int jk = 0 ; jk < MAX_CASES[ 0 ] ; jk++ ) {//dpc 5/1/13 vortex
				Yij += PseudoValues[ i ][ j ][ jk ] ;
			}
			Yij /= (double)( MAX_CASES[ 0 ]) ;//dpc 5/1/13 vortex
			for( int jk = 0 ; jk < MAX_CASES[ 0 ]; jk++ ) {//dpc 5/1/13 vortex
				PseudoValues[ i ][ j ][ jk ] = PseudoValues[ i ][ j ][ jk ] + (float)( TR_MEANS[ i ][ j ] - Yij ) ;
			}
		}
	}

	clear_fom_arr( N_TREATMENTS, MAX_READERS ) ;
	delete[] NL_jk ;
	delete[] LL_jk ;
	delete[] N_Lesions_jk ;
	delete[] weights_jk ;

	return ;
}


extern "C" JAFROC_CORE_API void __cdecl jackknife_FROC_Data_Abnormals( //dpc 5/1/13 vortex
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Image,
	int analysis_method,  float **weights,
	float **TR_MEANS, float ***PseudoValues, int *err )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	*err = 0 ;

	if( NCASES_ABN == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_ABN ;
	}
	if( ( analysis_method != ANALYSIS_METHOD_JAFROC1 && analysis_method != ANALYSIS_METHOD_JAFROC1_WEIGHTED ) && NCASES_NOR == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_NOR ;
	}

	if( MAX_NL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_NL ;
	}
	if( MAX_LL == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_N_MAX_LL ;
	}
	if( N_TREATMENTS == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_TREATMENT ;
	}
	if( MAX_READERS == 0 ) {
		*err |= JAFROC_DLL_ERROR_ZERO_READER ;
	}

	if( *err != 0 ) {
		return ;
	}

	calculate_treatment_means( 
		N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Image, weights, analysis_method,
		TR_MEANS, err ) ;
	alloc_fom_arr( N_TREATMENTS, MAX_READERS, MAX_CASES ) ;

	float **NL_jk, **LL_jk ;
	float **weights_jk ;
	int *N_Lesions_jk ;
	int MAX_CASES_jk[ 2 ] ;

	NL_jk = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ]  ] ;//dpc 5/1/13 vortex
	LL_jk = new float *[ MAX_CASES[ 1 ] - 1 ] ;
	weights_jk = new float *[ MAX_CASES[ 1 ] - 1] ;
	N_Lesions_jk = new int[ MAX_CASES[ 1 ] - 1 ] ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int jk = MAX_CASES[ 0 ] ; jk < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; jk++ ) {
				// build a jackknifed NL array
				for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					if( k < jk ) {
						// doesn't copy actual data, just assigning pointer of the original data
						NL_jk[ k ] = NL[ i ][ j ][ k ] ;
					} else if( k > jk ) {
						NL_jk[ k - 1] = NL[ i ][ j ][ k ] ;
					}
				}

				// jackknifed LL array
				if( jk >= MAX_CASES[ 0 ] ) {
					for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
						if( k < jk ) {
							LL_jk[ k - MAX_CASES[ 0 ] ] = LL[ i ][ j ][ k - MAX_CASES[ 0 ] ] ;
							weights_jk[ k - MAX_CASES[ 0 ] ] = weights[ k - MAX_CASES[ 0 ] ] ;
							N_Lesions_jk[ k - MAX_CASES[ 0 ] ] = N_Lesions_Per_Image[ k - MAX_CASES[ 0 ] ] ;
						} else if( k > jk ) {
							LL_jk[ k - MAX_CASES[ 0 ] - 1 ] = LL[ i ][ j ][ k - MAX_CASES[ 0 ] ] ;
							weights_jk[ k - MAX_CASES[ 0 ] - 1 ] = weights[ k - MAX_CASES[ 0 ] ] ;
							N_Lesions_jk[ k - MAX_CASES[ 0 ] - 1 ] = N_Lesions_Per_Image[ k - MAX_CASES[ 0 ] ] ;
						}
					}
				} else {
					for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
						LL_jk[ k - MAX_CASES[ 0 ] ] = LL[ i ][ j ][ k - MAX_CASES[ 0 ] ] ;
						weights_jk[ k - MAX_CASES[ 0 ] ] = weights[ k - MAX_CASES[ 0 ] ] ;
						N_Lesions_jk[ k - MAX_CASES[ 0 ] ] = N_Lesions_Per_Image[ k - MAX_CASES[ 0 ] ] ;
					}
				}
				// MAX_CASES_jk
				if( jk >= MAX_CASES[ 0 ] ) {
					MAX_CASES_jk[ 0 ] = MAX_CASES[ 0 ] ;
					MAX_CASES_jk[ 1 ] = MAX_CASES[ 1 ] - 1 ;
				} else {
					MAX_CASES_jk[ 0 ] = MAX_CASES[ 0 ] - 1 ;
					MAX_CASES_jk[ 1 ] = MAX_CASES[ 1 ] ;
				}

				// calculate FOM
				FOM_ARR[ i ][ j ][ jk ] = MyFOM( NL_jk, LL_jk, N_Lesions_jk, MAX_CASES_jk, MAX_NL, MAX_LL, weights_jk, analysis_method ) ;

				int fpc = _fpclass( FOM_ARR[ i ][ j ][ jk ] ) ;
				if( fpc == _FPCLASS_SNAN || fpc == _FPCLASS_QNAN || fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF ) {
					*err |= JAFROC_DLL_ERROR_FLOAT_ERROR ;
				}

				// calculate pseudo values
				PseudoValues[ i ][ j ][ jk - MAX_CASES[ 0 ] ] = (float)( 
					(double)( TR_MEANS[ i ][ j ] ) * (double)( MAX_CASES[ 1 ] ) -
					(double)( FOM_ARR[ i ][ j ][ jk ] ) * (double)( MAX_CASES[ 1 ] - 1 ) ) ;
			}
		}
	}

	// normalized pseudo values
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			double Yij = 0.0 ;
			for( int jk = 0 ; jk < MAX_CASES[ 1 ] ; jk++ ) {//dpc 5/1/13 vortex
				Yij += PseudoValues[ i ][ j ][ jk ] ;
			}
			Yij /= (double)( MAX_CASES[ 1 ]) ;//dpc 5/1/13 vortex
			for( int jk = 0 ; jk < MAX_CASES[ 1 ]; jk++ ) {//dpc 5/1/13 vortex
				PseudoValues[ i ][ j ][ jk ] = PseudoValues[ i ][ j ][ jk ] + (float)( TR_MEANS[ i ][ j ] - Yij ) ;
			}
		}
	}

	clear_fom_arr( N_TREATMENTS, MAX_READERS ) ;
	delete[] NL_jk ;
	delete[] LL_jk ;
	delete[] N_Lesions_jk ;
	delete[] weights_jk ;

	return ;
}

void alloc_analysis_memory( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
						 float ***TR_MEANS_in, float ****PseudoValues_in, float ***TRT_CI_in )
{
	float **TR_MEANS = new float *[ N_TREATMENTS ] ;
	float ***PseudoValues = new float **[ N_TREATMENTS ] ;
	float **TRT_CI = new float *[ N_TREATMENTS ] ; 
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		TR_MEANS[ i ] = new float[ MAX_READERS ] ;
		PseudoValues[ i ] = new float *[ MAX_READERS ] ;
		TRT_CI[ i ] = new float[ 2 ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			PseudoValues[ i ][ j ] = new float[ NCASES_NOR + NCASES_ABN ] ;
		}
	}
	*TR_MEANS_in = TR_MEANS ;
	*PseudoValues_in = PseudoValues ;
	*TRT_CI_in = TRT_CI ;
}

void free_analysis_memory( int N_TREATMENTS, int MAX_READERS,
						float **TR_MEANS, float ***PseudoValues, float **TRT_CI )
{
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] PseudoValues[ i ][ j ] ;
		}
		delete[] TR_MEANS[ i ] ;
		delete[] PseudoValues[ i ] ;
		delete[] TRT_CI[ i ] ;
	}
	delete[] TR_MEANS ;
	delete[] PseudoValues ;
	delete[] TRT_CI ;
}

extern "C" JAFROC_CORE_API void __cdecl Allocate_analysis_memory(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float ***TR_MEANS, float ****PseudoValues, float ***TRT_CI
	)
{
	alloc_analysis_memory( N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, TR_MEANS, PseudoValues, TRT_CI ) ;
}

extern "C" JAFROC_CORE_API void __cdecl Free_analysis_memory(
	int N_TREATMENTS, int MAX_READERS,
	float **TR_MEANS, float ***PseudoValues, float **TRT_CI
	)
{
	free_analysis_memory( N_TREATMENTS, MAX_READERS, TR_MEANS, PseudoValues, TRT_CI ) ;
}


extern "C" JAFROC_CORE_API void __cdecl Get_JAFROC_CORE_DLL_Timestamp( char *str )
{
	strcpy( str, __DATE__ ) ;
	strcat( str, " " ) ;
	strcat( str, __TIME__ ) ;
}
