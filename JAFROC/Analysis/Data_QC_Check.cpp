#include "stdafx.h"
#include "../common.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool Data_QC_Check( void )
{

	// 1. Fail if no abnormal cases exist
	if( MAX_CASES[ 1 ] == 0 ) {
		WriteToConsole( "No abnormal cases in the dataset; cannot proceed.\r\n\r\n" ) ;
		return false ;
	}

	if( MAX_CASES[ 0 ] != 0 ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				float **NL_ij = NL[ i ][ j ] ;
				float **LL_ij = LL[ i ][ j ] ;

				float *FP_ij = new float[ MAX_CASES[ 0 ] ] ;
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					FP_ij[ k ] = maximum( NL_ij[ k ], MAX_NL ) ;
				}
				int len_c1, len_c2 ;
				int *c1 = new int[ MAX_CASES[ 0 ] ] ;
				int *c2 = new int[ MAX_CASES[ 1 ] * MAX_LL ] ;
				where( FP_ij, MAX_CASES[ 0 ], "ne", UNINITIALIZED, c1, &len_c1 ) ;
				where( LL_ij, MAX_CASES[ 1 ], MAX_LL, "ne", UNINITIALIZED, c2, &len_c2 ) ;

				delete[] c1 ;
				delete[] c2 ;

				if( len_c1 == 0 ) {
					char str[ 200 ] ;
					sprintf_s( str, "Reader: %s made no non-lesion localizations on normal cases in treatment: %s\r\n\r\n",
						readerTable[ j ], modalityTable[ i ] ) ;
					WriteToConsole( str ) ;
/*					WriteToConsole( "This reader must be removed from analysis.\r\n" ) ;
					WriteToConsole( "Cannot proceed.\r\n\r\n" ) ;
					return false ; */
				}
				if( len_c2 == 0 ) {
					char str[ 200 ] ;
					sprintf_s( str, "Reader: %s made no lesion localization in treatment: %s\r\n\r\n",
						readerTable[ j ], modalityTable[ i ] ) ;
					WriteToConsole( str ) ;
/*					WriteToConsole( "This reader must be removed from analysis.\r\n" ) ;
					WriteToConsole( "Cannot proceed.\r\n" ) ;
					return false ; */
				}
				delete[] FP_ij ;
			}
		}
	} else {
		if( analysis_method != ANALYSIS_METHOD_JAFROC1 ) {
			WriteToConsole( "No Normal cases; must use JAFROC1; cannot proceed.\r\n\r\n" ) ;
			return false ;
		}
		if( MAX_CASES[ 0 ] != 0  ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			float **NL_ij = NL[ i ][ 0 ] ;
			float **LL_ij = LL[ i ][ 0 ] ;

			float *FP_ij = new float[ MAX_CASES[ 0 ] ] ;
			for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
				FP_ij[ k ] = maximum( NL_ij[ k ], MAX_NL ) ;
			}

			int len_c1, len_c2 ;
			int *c1 = new int[ MAX_CASES[ 0 ] ] ;
			int *c2 = new int[ MAX_CASES[ 0 ] * MAX_LL ] ;
			where( FP_ij, MAX_CASES[ 0 ], "ne", UNINITIALIZED, c1, &len_c1 ) ;
			where( LL_ij, MAX_CASES[ 1 ], MAX_LL, "ne", UNINITIALIZED, c2, &len_c2 ) ;

			delete[] c1 ;
			delete[] c2 ;

			if( len_c1 == 0 ) {
				char str[ 200 ] ;
				sprintf_s( str, "No false positives in treatment: %s\r\n\r\n",
					modalityTable[ i ] ) ;
				WriteToConsole( str ) ;
/*				WriteToConsole( "Cannot proceed.\r\n\r\n" ) ;
				return false ; */
			}
			if( len_c2 == 0 ) {
				char str[ 200 ] ;
				sprintf_s( str, "No lesion localizations in treatment: %s\r\n\r\n",
					modalityTable[ i ] ) ;
				WriteToConsole( str ) ;
/*				WriteToConsole( "Cannot proceed.\r\n\r\n" ) ;
				return false ; */
			}
		}
		}
	}

	if( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] < MINIMUM_NUMBER_OF_CASES ) {
		char str[ 200 ] ;
		sprintf_s( str, "Your dataset has less than %d cases.\r\n",
			MINIMUM_NUMBER_OF_CASES ) ;
		WriteToConsole( str ) ;
		WriteToConsole( "Do not attempt to generalize to cases: report random reader analysis only.\r\n\r\n" ) ;
	}

	bin_and_cumulate_afroc_data() ;

	float min_value = 0.10f ;
	int **fpf_flags = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fpf_flags[ i ] = new int[ MAX_READERS ] ;
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
//			if( maximum( FPF_BIN_AFROC[ i ][ j ], MAX_CASES[ 0 ] ) < min_value )
			if( maximum( FPF_BIN_AFROC[ i ][ j ], MAX_CTFFS ) < min_value )
				fpf_flags[ i ][ j ] = 1 ;
		}
	}

	if( 0 ) {
	if( total( fpf_flags, N_TREATMENTS, MAX_READERS ) != 0 ) {
		char str[ 200 ] ;
		sprintf_s( str, "For modalities and reader(s) listed below the maximum FPF is less than %g\r\n",
			min_value ) ;
		WriteToConsole( str ) ;
		WriteToConsole( "For them the JAFROC FOM includes a large extrapolation to (1,1).\r\n" ) ;

		int count = 3 ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( fpf_flags[ i ][ j ] != 0 ) {
					char str[ 200 ] ;
					sprintf_s( str, "Modality = %s, Reader = %s\r\n",
						modalityTable[ i ], readerTable[ j ] ) ;
					WriteToConsole( str ) ;
				}
			}
		}

		/* log file is not implemented yet, hjy
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( fpf_flags[ i ][ j ] != 0 ) {
					CString str ;
					str.Format( "WARNING: For treatment = %s, and Reader = %s, max FPF = %d\r\n",
						modalityTable[ i ], readerTable[ j ], maximum( FPF_BIN_AFROC[ i ][ j ], MAX_CASES[ 0 ] ) ) ;
				}
			}
		}
		*/
	}
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] fpf_flags[ i ] ;
	}
	delete[] fpf_flags ;


	if( MAX_CASES[ 0 ] != 0 ) {
		float fraction_normal = (float)MAX_CASES[ 0 ] / (float)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;
		if( fraction_normal < min_value && analysis_method != ANALYSIS_METHOD_JAFROC1 ) {
			char str[ 200 ] ;
			sprintf_s( str, "Fraction of normal cases to total # cases is less than %g\r\n",
				min_value ) ;
			WriteToConsole( str ) ;
			WriteToConsole( "Validity of JAFROC has not been investigated under such unbalanced conditions.\r\n" ) ;
			WriteToConsole( "May wish to use JAFROC1 after deleting all normal cases.\r\n" ) ;
		}
		int count_NL_ALL_0 = 0 ;
		int count_NL_ALL_1 = 0 ;
		int count_LL_ALL = 0 ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					int c1 = 0 ;
					for( int l = 0 ; l < MAX_NL ; l++ ) if( NL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) c1++ ;
					if( c1 > 0 ) count_NL_ALL_0 += c1 ;
				}
				for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					int c1 = 0 ;
					for( int l = 0 ; l < MAX_NL ; l++ ) if( NL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) c1++ ;
					if( c1 > 0 ) count_NL_ALL_1 += c1 ;
				}
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					int c1 = 0 ;
					for( int l = 0 ; l < MAX_LL ; l++ ) if( LL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) c1++ ;
					if( c1 > 0 ) count_LL_ALL += c1 ;
				}
			}
		}

		float f_nl_nor = (float)count_NL_ALL_0 / (float)MAX_CASES[ 0 ] / (float)N_TREATMENTS / (float)MAX_READERS ;
		float f_nl_abn = (float)count_NL_ALL_1 / (float)MAX_CASES[ 1 ] / (float)N_TREATMENTS / (float)MAX_READERS ;

		min_value = 0.3f ;
		float f_nl_abn_nor = f_nl_abn / f_nl_nor ;
		if( f_nl_abn_nor < min_value && analysis_method == ANALYSIS_METHOD_JAFROC1 ) {
			char str[ 200 ] ;
			sprintf_s( str, "Ratio of non lesion marks on abnormals to that on normals = %g\r\n", 
				f_nl_abn_nor ) ;
			WriteToConsole( str ) ;
			sprintf_s( str, "FP marks on abnormal images occur at less than %g the rate on normals\r\n", 
				min_value ) ;
			WriteToConsole( str ) ;
			WriteToConsole( "Validity of JAFROC1 has not been investigated under such unbalanced conditions.\r\n" ) ;
			WriteToConsole( "Suggest switching to JAFROC analysis instead.\r\n" ) ;
		}
	}
	return true ;
}