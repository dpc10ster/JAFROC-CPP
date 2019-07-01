#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"
#include <memory.h>
#include <stdlib.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int readline( FILE *stream, char *str )
{
	int c = fgetc( stream ), i = 0 ;
	if( c == EOF ) {
		str[ 0 ] = 0 ;
		return EOF ;
	}
	while( c != '\r' && c != '\n' ) {
		str[ i++ ] = c ;
		c = fgetc( stream ) ;
		if( c == EOF ) {
			str[ i ] = 0 ;
			return EOF ;
		}
	}
	str[ i ] = 0 ;
	return 0 ;
}

int isNumeric( char *source )
{
	int hit = 0 ;
	for( int i = 0 ; i < strlen( source ) ; i++ ) {
		if( source[ i ] == 0x20 ||
			source[ i ] == 0x2D ||
			source[ i ] == 0x2E ||
			source[ i ] == 0x45 ||
			source[ i ] == 0x65 ||
			( source[ i ] >= 0x30 && source[ i ] <= 0x39 ) ) {
		} else {
			hit = 1 ;
		}
	}
	return hit ;
}

int isInteger( char *source )
{
	int hit = 0 ;
	for( int i = 0 ; i < strlen( source ) ; i++ ) {
		if( ( source[ i ] >= 0x30 && source[ i ] <= 0x39 ) ) {
		} else {
			hit = 1 ;
		}
	}
	return hit ;
}

int GetMaxNumberJAFROC( char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL, 
	CStringArray &caseArray, CStringArray &readerArray, CStringArray &modalityArray, char *errorString )
{
	FILE *stream = fopen( filename, "rt" ) ;
	if( stream == NULL ) {
		sprintf( errorString, "Cannot open file." ) ;
		return JAFROC_DLL_ERROR_XL_FILE_READ ;
	}

	*MAX_NL = 0 ;
	*MAX_LL = 0 ;
	*NCASES_NOR = 0 ;
	*NCASES_ABN = 0 ;

	int lineindex = 0 ;
	char line[ 200 ] ;
	char r[ 80 ], m[ 80 ], c[ 80 ] ;
	char str[ 80 ] ;
	int n_nl, n_ll, n_t ;
	int ret ;
	do {
		ret = readline( stream, line ) ;
		lineindex++ ;
		char *ptr = line ;

		if( ptr[ 0 ] == ';' ) continue ;
		if( strlen( ptr ) == 0 ) continue ;

		while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;

		if( sscanf( ptr, "%s", r ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		{
			int hit = 0 ;
			for( int i = 0 ; i < readerArray.GetCount() ; i++ ) {
				if( readerArray.GetAt(i).Compare( r ) == 0 ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				readerArray.Add( r ) ;
			}
		}
		ptr += strlen( r ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( sscanf( ptr, "%s", m ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		{
			int hit = 0 ;
			for( int i = 0 ; i < modalityArray.GetCount() ; i++ ) {
				if( modalityArray.GetAt(i).Compare( m ) == 0 ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				modalityArray.Add( m ) ;
			}
		}
		ptr += strlen( m ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( sscanf( ptr, "%s", c ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		bool isNewCase = true ;
		{
			int hit = 0 ;
			for( int i = 0 ; i < caseArray.GetCount() ; i++ ) {
				if( caseArray.GetAt(i).Compare( c ) == 0 ) {
					hit = 1 ;
					isNewCase = false ;
					break ;
				}
			}
			if( hit == 0 ) {
				caseArray.Add( c ) ;
			}
		}
		ptr += strlen( c ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;

		if( sscanf( ptr, "%s", str ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( isInteger( str ) ) {
			sprintf( errorString, "Invalid number of truths at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		n_t = atoi( str ) ;

		if( sscanf( ptr, "%s", str ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( isInteger( str ) ) {
			sprintf( errorString, "Invalid number of lesion localizations at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		n_ll = atoi( str ) ;
		if( n_ll > n_t ) {
			sprintf( errorString, "More lesion localizations than number of truths at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		if( n_ll > *MAX_LL ) *MAX_LL = n_ll ;
		for( int i = 0 ; i < n_ll ; i++ ) {
			if( sscanf( ptr, "%s", str ) == EOF ) {
				sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
			if( isNumeric( str ) ) {
				sprintf( errorString, "Invalid lesion localization rating at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			// valid lesion localization rating
		}

		if( sscanf( ptr, "%s", str ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( isInteger( str ) ) {
			sprintf( errorString, "Invalid number of non-lesion localizations at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		n_nl = atoi( str ) ;
		if( n_nl > *MAX_NL ) *MAX_NL = n_nl ;
		for( int i = 0 ; i < n_nl ; i++ ) {
			if( sscanf( ptr, "%s", str ) == EOF ) {
				sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
			if( isNumeric( str ) ) {
				sprintf( errorString, "Invalid non-lesion localization rating at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			// valid non-lesion localization rating
		}
		if( isNewCase ) {
			if( n_t == 0 ) {
				*NCASES_NOR = *NCASES_NOR + 1 ;
			} else {
				*NCASES_ABN = *NCASES_ABN + 1 ;
			}
		}
	} while( ret != EOF ) ;
	fclose( stream ) ;

	*N_TREATMENTS = modalityArray.GetCount() ;
	*MAX_READERS = readerArray.GetCount() ;

	if( *NCASES_ABN == 0 ) {
		sprintf( errorString, "No abnormal cases in the dataset" ) ;
		return JAFROC_DLL_ERROR_XL_FILE_READ ;
	}

	return 0 ;
}


int ReadFROCDataJAFROC( 
	char *filename, int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights, 
	CStringArray &caseArray, CStringArray &readerArray, CStringArray &modalityArray, char *errorString )
{
	FILE *stream = fopen( filename, "rt" ) ;
	if( stream == NULL ) {
		sprintf( errorString, "Cannot open file." ) ;
		return JAFROC_DLL_ERROR_XL_FILE_READ ;
	}

	// for checking number of lesions
	int ***nles_array = new int **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		nles_array[ i ] = new int *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			nles_array[ i ][ j ] = new int[ MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ] ;
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				nles_array[ i ][ j ][ k ] = UNINITIALIZED ;
			}
		}
	}

	int lineindex = 0 ;
	char line[ 200 ] ;
	char r[ 80 ], m[ 80 ], c[ 80 ] ;
	char str[ 80 ] ;
	int n_nl, n_ll, n_t ;
	int ret ;
	do {
		ret = readline( stream, line ) ;
		lineindex++ ;
		char *ptr = line ;

		if( ptr[ 0 ] == ';' ) continue ;
		if( strlen( ptr ) == 0 ) continue ;

		while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;

		if( sscanf( ptr, "%s", r ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		int reader_index ;
		{
			int hit = 0 ;
			for( int i = 0 ; i < readerArray.GetCount() ; i++ ) {
				if( readerArray.GetAt(i).Compare( r ) == 0 ) {
					hit = 1 ;
					reader_index = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				sprintf( errorString, "Invalid reader ID at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;				
			}
		}
		ptr += strlen( r ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( sscanf( ptr, "%s", m ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		int modality_index ;
		{
			int hit = 0 ;
			for( int i = 0 ; i < modalityArray.GetCount() ; i++ ) {
				if( modalityArray.GetAt(i).Compare( m ) == 0 ) {
					hit = 1 ;
					modality_index = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				sprintf( errorString, "Invalid modality ID at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;				
			}
		}
		ptr += strlen( m ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( sscanf( ptr, "%s", c ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		int case_index ;
		{
			int hit = 0 ;
			for( int i = 0 ; i < caseArray.GetCount() ; i++ ) {
				if( caseArray.GetAt(i).Compare( c ) == 0 ) {
					hit = 1 ;
					case_index = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				sprintf( errorString, "Invalid case ID at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;				
			}
		}
		ptr += strlen( c ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( sscanf( ptr, "%s", str ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( isInteger( str ) ) {
			sprintf( errorString, "Invalid number of truths at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		n_t = atoi( str ) ;
		N_Lesions_Per_Case[ case_index ] = n_t ;
		// entry duplication error checking
		if( nles_array[ modality_index ][ reader_index ][ case_index ] != UNINITIALIZED ) {
			sprintf( errorString, "Reader %s modality %s case %s duplicated at line %d", 
				readerArray.GetAt( reader_index ), modalityArray.GetAt( modality_index ), caseArray.GetAt( case_index ), lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		nles_array[ modality_index ][ reader_index ][ case_index ] = n_t ;
		if( sscanf( ptr, "%s", str ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( isInteger( str ) ) {
			sprintf( errorString, "Invalid number of lesion localizations at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		n_ll = atoi( str ) ;
		for( int i = 0 ; i < n_ll ; i++ ) {
			if( sscanf( ptr, "%s", str ) == EOF ) {
				sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
			if( isNumeric( str ) ) {
				sprintf( errorString, "Invalid lesion localization rating at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			// valid non-lesion localization rating
			LL[ modality_index ][ reader_index ][ case_index ][ i ] = (float)atof( str ) ;
		}

		if( sscanf( ptr, "%s", str ) == EOF ) {
			sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
		if( isInteger( str ) ) {
			sprintf( errorString, "Invalid number of non-lesion localizations at line %d", lineindex ) ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
		n_nl = atoi( str ) ;
		for( int i = 0 ; i < n_nl ; i++ ) {
			if( sscanf( ptr, "%s", str ) == EOF ) {
				sprintf( errorString, "Invalid JAFROC data file format at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			ptr += strlen( str ) ; while( ( *ptr == ' ' || *ptr == '\t' ) && *ptr != '\r' && *ptr != '\n' && strlen( ptr ) != 0 ) ptr++ ;
			if( isNumeric( str ) ) {
				sprintf( errorString, "Invalid non-lesion localization rating at line %d", lineindex ) ;
				return JAFROC_DLL_ERROR_XL_FILE_READ ;
			}
			// valid lesion localization rating
			NL[ modality_index ][ reader_index ][ case_index ][ i ] = (float)atof( str ) ;
		}
	} while( ret != EOF ) ;
	fclose( stream ) ;

	// number of lesions error checking
	for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
		int hit = 0 ;
		int value = nles_array[ 0 ][ 0 ][ k ] ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( value != nles_array[ i ][ j ][ k ] ) {
					hit = 1 ;
				}
			}
		}
		if( hit == 1 ) {
			sprintf( errorString, "Mismatched number of lesions for case %s", caseArray.GetAt( k ) ) ;
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					delete[] nles_array[ i ][ j ] ;
				}
				delete[] nles_array[ i ] ;
			}
			delete[] nles_array ;
			return JAFROC_DLL_ERROR_XL_FILE_READ ;
		}
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] nles_array[ i ][ j ] ;
		}
		delete[] nles_array[ i ] ;
	}
	delete[] nles_array ;

	return 0 ;
}



extern "C" JAFROC_CORE_API void __cdecl JAFROC_V1_Loader( 
	char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL, 
	float *****NL, float *****LL, int **N_Lesions_Per_Case, float ***Weights, 
	char ***readerTable, char ***modalityTable, int *err, char *errorString )
{

	CStringArray caseStr, readerStr, modalityStr ;

	*err = 0 ;
	if( GetMaxNumberJAFROC( filename, N_TREATMENTS, MAX_READERS, NCASES_NOR, NCASES_ABN, MAX_NL, MAX_LL, 
		caseStr, readerStr, modalityStr, errorString ) ) {
		*err |= JAFROC_DLL_ERROR_XL_FILE_READ ;
		return ;
	}

	int MAX_CASES[ 2 ] ;
	MAX_CASES[ 0 ] = *NCASES_NOR ;
	MAX_CASES[ 1 ] = *NCASES_ABN ;

	init_data( *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, NL, LL, N_Lesions_Per_Case, Weights ) ;

	if( ReadFROCDataJAFROC( filename, *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, 
		*NL, *LL, *N_Lesions_Per_Case, *Weights, caseStr, readerStr, modalityStr, errorString ) ) {
//		clear_data_temp( *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, *NL, *LL, *N_Lesions_Per_Case, *Weights ) ;
		*err |= JAFROC_DLL_ERROR_XL_FILE_READ ;
		return ;
	}

	if( organize_cases( *N_TREATMENTS, *MAX_READERS, MAX_CASES, *MAX_NL, *MAX_LL, NL, LL, N_Lesions_Per_Case, Weights ) ) {
		*err |= JAFROC_DLL_ERROR_XL_FILE_READ ;
		strcpy( errorString, "JAFROC V1 Loader error" ) ;
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