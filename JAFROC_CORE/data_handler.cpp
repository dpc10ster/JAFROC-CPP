#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern char str[ 200 ] ;

/*
float ****NL = NULL ;
float ****LL = NULL ;
int *N_Lesions_Per_Image ;
float **Weights ;
*/
//int *ModalityArray, *ReaderArray, *CaseArray ;
/*
int N_TREATMENTS ;
int MAX_READERS ;
int MAX_CASES[ 2 ] ;
int MAX_LL, MAX_NL ;
*/


//
// initialize memory space to contain NL and LL marks
//
// Note that the number of cases for LL defined here is the same as NL
// becase the reason that we do not know how normal and abnormal cases are located.
// So we read them all and sort them to be located normal cases first and abnormal cases next,
// and remove unnecessary blocks of LL.
//
void init_data( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float *****NL, float *****LL, int **N_Lesions_Per_Image, float ***Weights )
{
	int i, j, k, l ;

//	clear_data() ;
	
	float ****NL_temp, ****LL_temp ;
	int *N_Les_temp ;
	float **Weights_temp ;

	NL_temp = new float ***[ N_TREATMENTS ] ;
	LL_temp = new float ***[ N_TREATMENTS ] ;
	for( i = 0 ; i < N_TREATMENTS ; i++ ) {
		NL_temp[ i ] = new float **[ MAX_READERS ] ;
		LL_temp[ i ] = new float **[ MAX_READERS ] ;
		for( j = 0 ; j < MAX_READERS ; j++ ) {
			NL_temp[ i ][ j ] = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
			LL_temp[ i ][ j ] = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
			for( k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				NL_temp[ i ][ j ][ k ] = new float[ MAX_NL ] ;
				for( l = 0 ; l < MAX_NL ; l++ ) {
					NL_temp[ i ][ j ][ k ][ l ] = UNINITIALIZED ;
				}
			}
			for( k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				LL_temp[ i ][ j ][ k ] = new float[ MAX_LL ] ;
				for( l = 0 ; l < MAX_LL ; l++ ) {
					LL_temp[ i ][ j ][ k ][ l ] = UNINITIALIZED ;
				}
			}
		}
	}
	N_Les_temp = new int[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
	Weights_temp = new float *[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
	for( i = 0 ; i < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; i++ ) {
		Weights_temp[ i ] = new float[ MAX_LL ] ;
		for( j = 0 ; j < MAX_LL ; j++ ) {
			Weights_temp[ i ][ j ] = 0.0f ;
		}
	}

	*NL = NL_temp ;
	*LL = LL_temp ;
	*N_Lesions_Per_Image = N_Les_temp ;
	*Weights = Weights_temp ;
}


void clear_data_temp( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights )
{
	int i, j, k ;

		for( i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( j = 0  ; j < MAX_READERS ; j++ ) {
				for( k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					delete[] NL[ i ][ j ][ k ] ;
				}
				delete[] NL[ i ][ j ] ;
				for( k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					delete[] LL[ i ][ j ][ k ] ;
				}
				delete[] LL[ i ][ j ] ;
			}
			delete[] NL[ i ] ;
			delete[] LL[ i ] ;
		}
		delete[] NL ;
		delete[] LL ;
		delete[] N_Lesions_Per_Case ;
		for( i = 0 ; i < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; i++ ) {
			delete[] Weights[ i ] ;
		}
		delete[] Weights ;
/*
		delete[] ModalityArray ;
		delete[] ReaderArray ;
		delete[] CaseArray ;
*/
		NL = NULL ;
		LL = NULL ;
}



void clear_data( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights )
{
	int i, j, k ;

	if( NL != NULL ) {
		for( i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( j = 0  ; j < MAX_READERS ; j++ ) {
				for( k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					delete[] NL[ i ][ j ][ k ] ;
				}
				delete[] NL[ i ][ j ] ;
				for( k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					delete[] LL[ i ][ j ][ k ] ;
				}
				delete[] LL[ i ][ j ] ;
			}
			delete[] NL[ i ] ;
			delete[] LL[ i ] ;
		}
		delete[] NL ;
		delete[] LL ;
		delete[] N_Lesions_Per_Case ;
		for( i = 0 ; i < MAX_CASES[ 1 ] ; i++ ) {
			delete[] Weights[ i ] ;
		}
		delete[] Weights ;
/*
		delete[] ModalityArray ;
		delete[] ReaderArray ;
		delete[] CaseArray ;
*/
		NL = NULL ;
		LL = NULL ;
	}
}


int organize_cases( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float *****NL, float *****LL, int **N_Lesions_Per_Image, float ***Weights )
{
	int *normal_cases, *abnormal_cases ;

	normal_cases = new int[ MAX_CASES[ 0 ] ] ;
	abnormal_cases = new int[ MAX_CASES[ 1 ] ] ;

	int nor_index = 0, abn_index = 0 ;
	for( int i = 0 ; i < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; i++ ) {
		if( (int *)(*N_Lesions_Per_Image)[ i ] == 0 ) {
			normal_cases[ nor_index++ ] = i ;
		} else {
			abnormal_cases[ abn_index++ ] = i ;
		}
	}
	if( nor_index != MAX_CASES[ 0 ] || abn_index != MAX_CASES[ 1 ] ) {
//		WriteToConsole( "Number of cases mismatched.\r\n" ) ;
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
					NL_temp[ i ][ j ][ k ][ l ] = ((float ****)(*NL))[ i ][ j ][ normal_cases[ k ] ][ l ] ;
				}
			}
			for( int k = MAX_CASES[ 0 ] ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_NL ; l++ ) {
					NL_temp[ i ][ j ][ k ][ l ] = ((float ****)(*NL))[ i ][ j ][ abnormal_cases[ k - MAX_CASES[ 0 ] ] ][ l ] ;
				}
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_LL ; l++ ) {
					LL_temp[ i ][ j ][ k ][ l ] = ((float ****)(*LL))[ i ][ j ][ abnormal_cases[ k ] ][ l ] ;
				}
			}
		}
	}

	// organize N_Cases array
	int *ncases_temp = new int[ MAX_CASES[ 1 ] ] ;
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		ncases_temp[ k ] = ((int *)(*N_Lesions_Per_Image))[ abnormal_cases[ k ] ] ;
	}
	// organize Weights array
	float **Weights_temp ;
	Weights_temp = new float *[ MAX_CASES[ 1 ] ] ;
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		Weights_temp[ k ] = new float[ MAX_LL ] ;
	}
	for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
		for( int l = 0 ; l < MAX_LL ; l++ ) {
			Weights_temp[ k ][ l ] = ((float **)(*Weights))[ abnormal_cases[ k ] ][ l ] ;
		}
	}

	// free memory
	clear_data_temp ( N_TREATMENTS, MAX_READERS, MAX_CASES, MAX_NL, MAX_LL, *NL, *LL, *N_Lesions_Per_Image, *Weights ) ;

	// switch back to the original NL and LL
	// at this point every normal cases up and abnormals down
	*NL = NL_temp ;
	*LL = LL_temp ;

	*N_Lesions_Per_Image = ncases_temp ;
	*Weights = Weights_temp ;

	// clean up
	delete[] normal_cases ;
	delete[] abnormal_cases ;

	return 0 ;
}

extern "C" JAFROC_CORE_API void __cdecl XLReader( 
	char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL, 
	float *****NL, float *****LL, int **N_Lesions_Per_Case, float ***Weights, 
	char ***readerTable, char ***modalityTable, int *err, char *errorString )
{
	CStringArray caseStr, readerStr, modalityStr ;

	*err = 0 ;
	if( GetMaxNumber( filename, N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, MAX_NL, MAX_LL ) ) {
		strcpy( errorString, str ) ;
		*err |= JAFROC_DLL_ERROR_XL_FILE_READ ;
		return ;
	}
	if( *err != 0 ) return ;

	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = *NCASES_NOR ;
	MAX_CASES[ 1 ] = *NCASES_ABN ;

	init_data( *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, NL, LL, N_Lesions_Per_Case, Weights ) ;

	if( ReadFROCDataXLS( filename, *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, 
		*NL, *LL, *N_Lesions_Per_Case, *Weights, caseStr, readerStr, modalityStr ) ) {
		*err |= JAFROC_DLL_ERROR_XL_FILE_READ ;
		strcpy( errorString, str ) ;
		return ;
	}

	if( organize_cases( *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, NL, LL, N_Lesions_Per_Case, Weights ) ) {
		*err |= JAFROC_DLL_ERROR_XL_FILE_READ ;
		strcpy( errorString, str ) ;
		return ;
	}

	int maxlen = 0 ;
	for( int i = 0 ; i < readerStr.GetCount() ; i++ ) {
		CString s = readerStr.GetAt( i ) ;
		if( s.GetLength() > maxlen ) {
			maxlen = s.GetLength() ;
		}
	}

	char **tempstr = new char *[ *MAX_READERS ] ;
	for( int i = 0 ; i < *MAX_READERS ; i++ ) {
		tempstr[ i ] = new char[ maxlen + 1 ] ;
		strcpy( tempstr[ i ], readerStr.GetAt( i ) ) ;
	}
	*readerTable = tempstr ;

	maxlen = 0 ;
	for( int i = 0 ; i < modalityStr.GetCount() ; i++ ) {
		CString s = modalityStr.GetAt( i ) ;
		if( s.GetLength() > maxlen ) {
			maxlen = s.GetLength() ;
		}
	}
	tempstr = new char *[ *N_TREATMENTS ] ;
	for( int i = 0 ; i < *N_TREATMENTS ; i++ ) {
		tempstr[ i ] = new char[ maxlen + 1 ] ;
		strcpy( tempstr[ i ], modalityStr.GetAt( i ) ) ;
	}
	*modalityTable = tempstr ;


	caseStr.RemoveAll() ;
	readerStr.RemoveAll() ;
	modalityStr.RemoveAll() ;

}


extern "C" JAFROC_CORE_API void __cdecl Free_FROC_Data( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights, 
	char **readerTable, char **modalityTable )
{
	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = NCASES_NOR ;
	MAX_CASES[ 1 ] = NCASES_ABN ;

	clear_data( N_TREATMENTS, MAX_READERS, MAX_CASES, MAX_NL, MAX_LL, NL, LL, N_Lesions_Per_Case, Weights ) ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] modalityTable[ i ] ;
	}
	delete[] modalityTable ;

	for( int i = 0 ; i < MAX_READERS ; i++ ) {
		delete[] readerTable[ i ] ;
	}
	delete[] readerTable ;
}