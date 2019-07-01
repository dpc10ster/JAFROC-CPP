#include "stdafx.h"
#include "../common.h"
#include <stdio.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern char *DisclaimerText , *versionStr;

char char_buffer_DLL[200], char_buffer_EXE[200];

void PrintDataSummaryInitial(FILE *report )
{
char Analysis_Method_Str[200];

	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_JAFROC1"); 
			break ;
		case ANALYSIS_METHOD_JAFROC:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_JAFROC"); 
			break ;
		case ANALYSIS_METHOD_HR_ROC:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_HR_ROC"); 
			break ;
		case ANALYSIS_METHOD_HR_SENSITIVITY:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_HR_SENSITIVITY"); 
			break ;
		case ANALYSIS_METHOD_HR_SPECIFICITY:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_HR_SPECIFICITY"); 
			break ;
		case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_JAFROC1_WEIGHTED"); 
			break ;
		case ANALYSIS_METHOD_JAFROC_WEIGHTED:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_JAFROC_WEIGHTED"); 
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A1:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_SONG_FOM_A1"); 
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A2:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_SONG_FOM_A2"); 
			break ;
		case ANALYSIS_METHOD_ROI:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_ROI"); 
			break ;
		case ANALYSIS_METHOD_MAX_NLF:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_MAX_NLF"); 
			break ;
		case ANALYSIS_METHOD_MAX_LLF:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_MAX_LLF"); 
			break ;		
		case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
			strcpy_s( Analysis_Method_Str, "ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY"); 
			break ;		
	}

	Get_JAFROC_CORE_DLL_Timestamp( char_buffer_DLL ) ;
	strcpy_s( char_buffer_EXE, __DATE__ ) ; strcat_s( char_buffer_EXE, " " ) ; strcat_s( char_buffer_EXE, __TIME__ ) ;
	CString str ; GetTimeStamp( &str ) ;

	char tempBuff[200]; 

	if (report != NULL) {
		fprintf( report, DisclaimerText) ;

		fprintf( report, "================================================================================\n" ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, versionStr ) ;
		fprintf( report, "Last Compilation Dates\n" ) ;
		fprintf( report, "\tJAFROC_CORE.DLL : %s\n", char_buffer_DLL ) ;
		fprintf( report, "\tJAFROC.EXE      : %s\n", char_buffer_EXE ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, "\tToday's date: %s\n", str ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, "FOM selected         :     %s\n", Analysis_Method_Str ) ;
		fprintf( report, "Input  Data Filename :     %s\n", fileName ) ;
		fprintf( report, "Output Data Filename :     %s\n", outfilename ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, "================================================================================\n" ) ;
	} else {		
		WriteToConsole( DisclaimerText) ;
		WriteToConsole( versionStr ) ;
		WriteToConsole( "Last Compilation Dates:\n" ) ;
		strcpy_s( tempBuff, "\tJAFROC_CORE.DLL :" ); 
		strcat_s( tempBuff, " " ) ; 
		strcat_s( tempBuff, char_buffer_DLL );
		WriteToConsole( tempBuff ) ;
		WriteToConsole( "\n" ) ;
		strcpy_s( tempBuff, "\tJAFROC.EXE :" ); 
		strcat_s( tempBuff, " " ) ; 
		strcat_s( tempBuff, char_buffer_EXE );
		WriteToConsole( tempBuff ) ;
		WriteToConsole(  "\n" ) ;
		strcpy_s( tempBuff, "\tToday's date: " ); 
		strcat_s( tempBuff, " " ) ; 
		strcat_s( tempBuff, str );
		WriteToConsole( tempBuff ) ;
		WriteToConsole(  "\n" ) ;
	} 

}


void PrintDataSummary( FILE *report )
{
	PrintDataSummaryInitial(report );

	float incorrect_localizations = 0.0f ;
	float ILF ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			float **NL_ij = NL[ i ][ j ] ;
			float **LL_ij = LL[ i ][ j ] ;
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				if( maximum( NL_ij[ k + MAX_CASES[ 0 ] ], MAX_NL ) > maximum( LL_ij[ k ], MAX_LL ) )
					incorrect_localizations += 1.0f ;
				else if( maximum( NL_ij[ k + MAX_CASES[ 0 ] ], MAX_NL ) > maximum( LL_ij[ k ], MAX_LL ) )
					incorrect_localizations += 0.5f ;
			}
		}
	}

	ILF = incorrect_localizations / (float)N_TREATMENTS / (float)MAX_READERS / (float)MAX_CASES[ 1 ] ;

	if( perform_bootstrap ) {
		switch( analysis_method ) {
			case ANALYSIS_METHOD_JAFROC1: 
				fprintf( report, " Analysis method:         :  JAFROC1 FOM BOOTSTRAP CONFIDENCE INTERVALS\n" ) ;
				break ;
			case ANALYSIS_METHOD_JAFROC:
				fprintf( report, " Analysis method:         :  JAFROC FOM BOOTSTRAP CONFIDENCE INTERVALS\n" ) ;
				break ;
			case ANALYSIS_METHOD_HR_ROC:
				fprintf( report, " Analysis method:         :  Inferred ROC FOM BOOTSTRAP CONFIDENCE INTERVALS\n" ) ;
				break ;
			case ANALYSIS_METHOD_JAFROC1_WEIGHTED: 
				fprintf( report, " Analysis method:         :  JAFROC1 FOM BOOTSTRAP CONFIDENCE INTERVALS\n" ) ;
				break ;
			case ANALYSIS_METHOD_JAFROC_WEIGHTED:
				fprintf( report, " Analysis method:         :  JAFROC FOM BOOTSTRAP CONFIDENCE INTERVALS\n" ) ;
				break ;
		}
	} else {
	switch( analysis_method ) {
			case ANALYSIS_METHOD_JAFROC1: 
				fprintf( report, " Analysis method:         :  JAFROC1 FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_JAFROC:
				fprintf( report, " Analysis method:         :  JAFROC FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_HR_ROC:
				fprintf( report, " Analysis method:         :  Inferred ROC FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_JAFROC1_WEIGHTED: 
				fprintf( report, " Analysis method:         :  WEIGHTED JAFROC1 FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_JAFROC_WEIGHTED:
				fprintf( report, " Analysis method:         :  WEIGHTED JAFROC FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1:
				fprintf( report, " Analysis method:         :  Song A1 FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A2:
				fprintf( report, " Analysis method:         :  Song A2 FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_MAX_NLF:
				fprintf( report, " Analysis method:         :  FOM = MAX_NLF DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_MAX_LLF:
				fprintf( report, " Analysis method:         :  FOM = MAX_LLF DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
				fprintf( report, " Analysis method:         :  FOM = INFERRED_SPECIFICITY DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_HR_SENSITIVITY:
				fprintf( report, " Analysis method:         :  FOM = ANALYSIS_METHOD_HR_SENSITIVITY Y DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_HR_SPECIFICITY:
				fprintf( report, " Analysis method:         :  FOM = ANALYSIS_METHOD_HR_SPECIFICITY DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
			case ANALYSIS_METHOD_ROI:
				fprintf( report, " Analysis method:         :  ROI FOM DBM-MRMC SIGNIFICANCE TESTING\n" ) ;
				break ;
		}
	}
	fprintf( report, " Number of Readers        :  %d\n", MAX_READERS ) ;
	fprintf( report, " Number of Treatments     :  %d\n", N_TREATMENTS ) ;
	fprintf( report, " Number of Normal Cases   :  %d\n", MAX_CASES[ 0 ] ) ;
	fprintf( report, " Number of Abnormal Cases :  %d\n", MAX_CASES[ 1 ] ) ;

	float fraction_normal = (float)MAX_CASES[ 0 ] / (float)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;

	fprintf( report, " fraction normal cases    :  %5.3f\n", fraction_normal ) ;
	fprintf( report, " min    LESIONS_PER_IMAGE :  %d\n", minimum( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
	fprintf( report, " max    LESIONS_PER_IMAGE :  %d\n", maximum( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
	fprintf( report, " mean   LESIONS_PER_IMAGE :  %g\n", mean( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
	fprintf( report, " Total  LESIONS           :  %d\n", total( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
/*	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( report, " Inc. Loc. Frac. Mod. %d   :  %5.3f\n", i, ILF[ i ] ) ;
	} */
	fprintf( report, " Inc. Loc. Frac.          :  %5.3f\n", ILF ) ;
	fprintf( report, "\n" ) ;

	if( MAX_CASES[ 0 ] != 0 ) {
	fprintf( report, "================================================================================\n" ) ;
	fprintf( report, "\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			char str[ 200 ], str1[ 200 ] ;
			sprintf_s( str, "For TRT = %-10s,", modalityTable[ i ] ) ;
			if( MAX_READERS != 1 ) {
				sprintf_s( str1, " and RDR = %-10s, max FPF = %6.3f.\n", 
					readerTable[ j ], 
					maximum( FPF_BIN_AFROC[ i ][ j ], number_of_bins_AFROC[ i ][ j ] ) 
					) ;
			} else {
				sprintf_s( str1, "                    , max FPF = %6.3f.\n", maximum( FPF_BIN_AFROC[ i ][ j ], number_of_bins_AFROC[ i ][ j ] ) ) ;
			}
			fprintf( report, "%s%s", str, str1 ) ;
		}
	}
	}

	fprintf( report, "\n" ) ;
	fprintf( report, "================================================================================\n" ) ;
	fprintf( report, "\n" ) ;

	
	// count number of LL marks per case averaged over all readers and modalities
	// count number of NL marks on normal vs. abnormal cases averaged over all readers and modalities
	if( 1 ) {
		int total_nl_1 = 0, total_nl_2 = 0, total_ll = 0;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				int nl_count = 0 ;
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					for( int l = 0 ; l < MAX_NL ; l++ ) {
						if( NL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) {
							nl_count++ ;
						}
					}
				}
				total_nl_1 += nl_count;

				nl_count = 0 ;
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					for( int l = 0 ; l < MAX_NL ; l++ ) {
						if( NL[ i ][ j ][ k + MAX_CASES[ 0 ] ][ l ] != UNINITIALIZED ) {
							nl_count++ ;
						}
					}
				}
				total_nl_2 += nl_count;

				int ll_count = 0 ;
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					for( int l = 0 ; l < MAX_LL ; l++ ) {
						if( LL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) {
							ll_count++ ;
						}
					}
				}
				total_ll += ll_count;
			}
		}
		fprintf( report, " Avg. number of non-lesion localization marks per reader on normal images: %6.3f.\n", 
			(total_nl_1+0.0)/MAX_READERS/N_TREATMENTS/MAX_CASES[ 0 ] ) ;
		fprintf( report, " Avg. number of non-lesion localization marks per reader on abnormal images: %6.3f.\n", 
			(total_nl_2+0.0)/MAX_READERS/N_TREATMENTS/MAX_CASES[ 1 ] ) ;
		fprintf( report, " Avg. number of lesion localization marks per reader : %6.3f.\n", (total_ll+0.0)/MAX_READERS/N_TREATMENTS/MAX_CASES[ 1 ] ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, "================================================================================\n" ) ;
		fprintf( report, "\n" ) ;
	}
	
}