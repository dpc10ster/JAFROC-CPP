#include "stdafx.h"
#include "../common.h"
#include <math.h>
#include <nr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


float Results[ 2 ], DF[ 2 ] ;
float *Left = NULL, *Right = NULL ;
float **TRT_CI_CASES = NULL ;
float **TRT_CI_ALL = NULL ;
float **TRT_CI_RDRS = NULL ;


float ***FOM_ARR = NULL ;
float ***PseudoValues = NULL ;



void clear_pseudovalues( void )
{
	if( PseudoValues != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				delete[] FOM_ARR[ i ][ j ] ;
				delete[] PseudoValues[ i ][ j ] ;
			}
			delete[] FOM_ARR[ i ] ;
			delete[] PseudoValues[ i ] ;
		}
		delete[] FOM_ARR ;
		delete[] PseudoValues ;
		PseudoValues = NULL ;
	}
}

void alloc_pseudovalues( void )
{
	clear_pseudovalues() ;

	FOM_ARR = new float **[ N_TREATMENTS ] ;
	PseudoValues = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		FOM_ARR[ i ] = new float *[ MAX_READERS ] ;
		PseudoValues[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			FOM_ARR[ i ][ j ] = new float[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
			PseudoValues[ i ][ j ] = new float[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
		}
	}
}


float var_tr, var_tc, var_trc ;
float **TR_MEANS = NULL ;

void clear_treatment_means( void )
{
	if( TR_MEANS != NULL ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			delete[] TR_MEANS[ i ] ;
		}
		delete[] TR_MEANS ;
		TR_MEANS = NULL ;
	}
}

void alloc_treatment_means( void )
{
	clear_treatment_means() ;

	TR_MEANS = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		TR_MEANS[ i ] = new float[ MAX_READERS ] ;
	}
}

void Alloc_DLL_Memory( void )
{
	alloc_pseudovalues() ;
	alloc_treatment_means() ;
}


void jackknife_FROC_Data_DLL( )
{
	int err ;
	jackknife_FROC_Data( N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Image, 
		analysis_method, Weights,
		TR_MEANS, PseudoValues, &err
		) ;

	if( err != 0 ) {
		WriteToConsole( "Error on jackknife_FROC_Data() function" ) ;
		if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
			WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
			WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
			WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
			WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
			WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
			WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
			WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
			WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
			WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
		}
	}

	
	if( MAX_READERS > 1 ) {

		if( err != 0 ) {
			WriteToConsole( "Error on mean_squares() function" ) ;
			if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
				WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
				WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
				WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
				WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
				WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
				WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
				WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
				WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
				WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
			}
		}

	}

}


void jackknife_FROC_Data_Normals_DLL( )
{
	int err ;
	jackknife_FROC_Data_Normals( N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Image, 
		analysis_method, Weights,
		TR_MEANS, PseudoValues, &err
		) ;

	if( err != 0 ) {
		WriteToConsole( "Error on jackknife_FROC_Data() function" ) ;
		if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
			WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
			WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
			WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
			WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
			WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
			WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
			WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
			WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
			WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
		}
	}

	
	if( MAX_READERS > 1 ) {
//		mean_squares() ;
		/*
		mean_squares(
			N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], PseudoValues,
			&MS_T, &MS_R, &MS_C, &MS_TC, &MS_RC, &MS_TR, &MS_TRC, &var_tr, &var_tc, &var_trc, &err
			) ;
			*/
		if( err != 0 ) {
			WriteToConsole( "Error on mean_squares() function" ) ;
			if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
				WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
				WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
				WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
				WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
				WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
				WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
				WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
				WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
				WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
			}
		}

	}

}


void jackknife_FROC_Data_Abnormals_DLL( )
{
	int err ;
	jackknife_FROC_Data_Abnormals( N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Image, 
		analysis_method, Weights,
		TR_MEANS, PseudoValues, &err
		) ;

	if( err != 0 ) {
		WriteToConsole( "Error on jackknife_FROC_Data() function" ) ;
		if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
			WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
			WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
			WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
			WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
			WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
			WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
			WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
			WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
			WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
		}
	}

	
	if( MAX_READERS > 1 ) {
//		mean_squares() ;
		/*
		mean_squares(
			N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], PseudoValues,
			&MS_T, &MS_R, &MS_C, &MS_TC, &MS_RC, &MS_TR, &MS_TRC, &var_tr, &var_tc, &var_trc, &err
			) ;
			*/
		if( err != 0 ) {
			WriteToConsole( "Error on mean_squares() function" ) ;
			if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
				WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
				WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
				WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
				WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
				WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
				WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
				WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
				WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
				WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
			}
		}

	}

}
