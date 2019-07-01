#include "stdafx.h"
#include "XLReader.h"
#include "../common.h"
#include <string.h>
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char **caseTable, **readerTable, **modalityTable ;
char *datafileDescriptionText ;


float ****NL = NULL ;
float ****LL = NULL ;
int *N_Lesions_Per_Image ;
float **Weights ;
int *ModalityArray, *ReaderArray, *CaseArray ;
int N_TREATMENTS ;
int MAX_READERS ;
int MAX_CASES[ 2 ] ;
int MAX_LL, MAX_NL ;



//
// initialize memory space to contain NL and LL marks
//
// Note that the number of cases for LL defined here is the same as NL
// becase the reason that we do not know how normal and abnormal cases are located.
// So we read them all and sort them to be located normal cases first and abnormal cases next,
// and remove unnecessary blocks of LL.
//
void clear_data( void )
{
	if( NL != NULL ) {
		Free_FROC_Data( N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], MAX_NL, MAX_LL, 
			NL, LL, N_Lesions_Per_Image, Weights, readerTable, modalityTable ) ;
		NL = NULL ;
		LL = NULL ;
	}
}

/*
int organize_cases( void )
{
	int *normal_cases, *abnormal_cases ;

	normal_cases = new int[ MAX_CASES[ 0 ] ] ;
	abnormal_cases = new int[ MAX_CASES[ 1 ] ] ;

	int nor_index = 0, abn_index = 0 ;
	for( int i = 0 ; i < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; i++ ) {
		if( N_Lesions_Per_Image[ i ] == 0 ) {
			normal_cases[ nor_index++ ] = i ;
		} else {
			abnormal_cases[ abn_index++ ] = i ;
		}
	}
	if( nor_index != MAX_CASES[ 0 ] || abn_index != MAX_CASES[ 1 ] ) {
		WriteToConsole( "Number of cases mismatched.\r\n" ) ;
		return 1 ;
	}
	
	// make temp array
	float ****NL_temp, ****LL_temp ;

	NL_temp = new float ***[ N_TREATMENTS ] ;
	LL_temp = new float ***[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		NL_temp[ i ] = new float **[ MAX_READERS ] ;
		LL_temp[ i ] = new float **[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			NL_temp[ i ][ j ] = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
			LL_temp[ i ][ j ] = new float *[ MAX_CASES[ 1 ] ] ;
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				NL_temp[ i ][ j ][ k ] = new float[ MAX_NL ] ;
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				LL_temp[ i ][ j ][ k ] = new float[ MAX_LL ] ;
			}
		}
	}
	
	// copy data to a new order
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
				for( int l = 0 ; l < MAX_NL ; l++ ) {
					NL_temp[ i ][ j ][ k ][ l ] = NL[ i ][ j ][ normal_cases[ k ] ][ l ] ;
				}
			}
			for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_NL ; l++ ) {
					NL_temp[ i ][ j ][ k ][ l ] = NL[ i ][ j ][ abnormal_cases[ k - MAX_CASES[ 0 ] ] ][ l ] ;
				}
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_LL ; l++ ) {
					LL_temp[ i ][ j ][ k ][ l ] = LL[ i ][ j ][ abnormal_cases[ k ] ][ l ] ;
				}
			}
		}
	}

	// free memory
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0  ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				delete[] NL[ i ][ j ][ k ] ;
				delete[] LL[ i ][ j ][ k ] ;
			}
			delete[] NL[ i ][ j ] ;
			delete[] LL[ i ][ j ] ;
		}
		delete[] NL[ i ] ;
		delete[] LL[ i ] ;
	}
	delete[] NL ;
	delete[] LL ;

	// switch back to the original NL and LL
	// at this point every normal cases up and abnormals down
	NL = NL_temp ;
	LL = LL_temp ;

	// organize N_Cases array
	int *ncases_temp = new int[ MAX_CASES[ 1 ] ] ;
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		ncases_temp[ k ] = N_Lesions_Per_Image[ abnormal_cases[ k ] ] ;
	}
	delete[] N_Lesions_Per_Image ;
	N_Lesions_Per_Image = ncases_temp ;

	// organize Weights array
	float **Weights_temp ;
	Weights_temp = new float *[ MAX_CASES[ 1 ] ] ;
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		Weights_temp[ k ] = new float[ MAX_LL ] ;
	}
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		for( int l = 0 ; l < MAX_LL ; l++ ) {
			Weights_temp[ k ][ l ] = Weights[ abnormal_cases[ k ] ][ l ] ;
		}
	}
	for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
		delete[] Weights[ k ] ;
	}
	delete[] Weights ;
	Weights = Weights_temp ;

	// organize Case array
	int *case_array_temp = new int[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
	for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
		case_array_temp[ k ] = CaseArray[ normal_cases[ k ] ] ;
	}
	for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
		case_array_temp[ k ] = CaseArray[ abnormal_cases[ k - MAX_CASES[ 0 ] ] ] ;
	}
	delete[] CaseArray ;
	CaseArray = case_array_temp ;

	// clean up
	delete[] normal_cases ;
	delete[] abnormal_cases ;

	return 0 ;
}
*/

bool read_jafroc_data( char *filename )
{
	char errorString[ 200 ] ;
	int err ;
	int NCASES_NOR, NCASES_ABN ;

	char *fileExt = filename ;
	fileExt += strlen( filename ) - 3 ;
	if( strcmp( fileExt, "txt" ) == 0 || strcmp( fileExt, "TXT" ) == 0 ) {
		JAFROC_V1_Loader( filename, &N_TREATMENTS, &MAX_READERS, &NCASES_NOR, &NCASES_ABN, &MAX_NL, &MAX_LL,
			&NL, &LL, &N_Lesions_Per_Image, &Weights, &readerTable, &modalityTable, &err, errorString ) ;
	} else {
		XLReader( filename, &N_TREATMENTS, &MAX_READERS, &NCASES_NOR, &NCASES_ABN, &MAX_NL, &MAX_LL,
			&NL, &LL, &N_Lesions_Per_Image, &Weights, &readerTable, &modalityTable, &err, errorString ) ;
		// data file description - hjy 3/31/11
		if( datafileDescriptionText == NULL ) {
			datafileDescriptionText = new char[ 32768 ] ;
		}
		ReadJAFROCDataFileDescription( filename, datafileDescriptionText ) ;
	}

	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	if( err != 0 ) {
		if( err == JAFROC_DLL_ERROR_XL_FILE_READ && strlen( errorString ) == 0 ) {
			WriteToConsole( "Excel data file format error." ) ;
		} else {
			WriteToConsole( errorString ) ;
		}
		WriteToConsole( "\r\n\r\n" ) ;
		return false ;
	}

	// if N_TREATMENTS < 1
	if( N_TREATMENTS < 2 ) {
		WriteToConsole( "Number of modality is less than two. Cannot proceed analysis.\r\n\r\n" ) ;
		return false ;
	}

	// report data status
	char str[ 200 ] ;
	sprintf_s( str, "Datafile: %s\r\n", filename ) ;
	WriteToConsole( str ) ;
	sprintf_s( str, "Number of TREATMENTS: %d\r\n", N_TREATMENTS ) ;
	WriteToConsole( str ) ;
	sprintf_s( str, "Number of READERS: %d\r\n", MAX_READERS ) ;
	WriteToConsole( str ) ;
	sprintf_s( str, "Number of NORMAL CASES: %d\r\n", MAX_CASES[ 0 ] ) ;
	WriteToConsole( str ) ;
	sprintf_s( str, "Number of ABNORMAL CASES: %d\r\n", MAX_CASES[ 1 ] ) ;
	WriteToConsole( str ) ;
	sprintf_s( str, "Maximum number of lesions per case: %d\r\n", MAX_LL ) ;
	WriteToConsole( str ) ;
	float total_n_les = 0.0f ;
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		total_n_les += (float)N_Lesions_Per_Image[ k ] ;
	}
	float avg_n_les = total_n_les / (float)MAX_CASES[ 1 ] ;
	sprintf_s( str, "Average number of lesions per case: %g\r\n", avg_n_les ) ;
	WriteToConsole( str ) ;
	sprintf_s( str, "Total number of lesions: %d\r\n\r\n", (int)total_n_les ) ;
	WriteToConsole( str ) ;

	if( datafileDescriptionText != NULL ) {
		if( strlen( datafileDescriptionText ) > 0 ) {
			WriteToConsole( "Summary:\r\n\r\n" ) ;
			WriteToConsole( datafileDescriptionText ) ;
			WriteToConsole( "\r\n\r\n" ) ;
		}
	}

	// preparing weights
	int weight_flag = 0 ;
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		float n_les = (float)N_Lesions_Per_Image[ k ] ;
		float sum = 0.0f ;
		for( int l = 0 ; l < (int)n_les ; l++ ) {
			sum += Weights[ k ][ l ] ;
		}
		if( fabs( sum - 1.0f ) > 1e-2 ) {
			weight_flag = 1;
			for( int l = 0 ; l < (int)n_les ; l++ ) {
				Weights[ k ][ l ] = 1.0f / n_les ;
			}
		}
	}
	if( weight_flag ) {
		WriteToConsole( "Equal weights assigned to some cases.\r\n\r\n" ) ;
	}

	return true ;
}

