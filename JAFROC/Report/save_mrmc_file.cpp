#include "stdafx.h"
#include "../common.h"
#include <stdio.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



bool save_mrmc_file( FILE *report, char *filename )
{
	FILE *stream ;
	errno_t err = fopen_s( &stream, filename, "wt" ) ;
	if( err != 0 ) {
		fprintf( report, "\n" ) ;
		fprintf( report, "================================================================================\n" ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, " Error generating LABMRMC format data file \n" ) ;
		fprintf( report, "\n" ) ;

		WriteToConsole( "Error generating LABMRMC data file.\r\n\r\n" ) ;

		return false ;
	}

	init_roc_data() ;

	float minv = -UNINITIALIZED ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
				FP[ i ][ j ][ k ] = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
				if( FP[ i ][ j ][ k ] < minv && FP[ i ][ j ][ k ] != UNINITIALIZED ) minv = FP[ i ][ j ][ k ] ;
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				float nl = maximum( NL[ i ][ j ][ k + MAX_CASES[ 0 ] ], MAX_NL ) ;
				float ll = maximum( LL[ i ][ j ][ k ], MAX_LL ) ;
				if( nl > ll ) TP[ i ][ j ][ k ] = nl ;
				else TP[ i ][ j ][ k ] = ll ;
				if( TP[ i ][ j ][ k ] < minv && TP[ i ][ j ][ k ] != UNINITIALIZED ) minv = TP[ i ][ j ][ k ] ;
			}
		}
	}

	// substitute UNINITIALIZED to ( minimum - 1 )
	minv -= 1.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
				if( FP[ i ][ j ][ k ] == UNINITIALIZED ) FP[ i ][ j ][ k ] = minv ;
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				if( TP[ i ][ j ][ k ] == UNINITIALIZED ) TP[ i ][ j ][ k ] = minv ;
			}
		}
	}

	int first = 1 ;
	fprintf( stream, "FROC data converted to LABMRMC format using highest rating on each image as the inferred-ROC rating\n" ) ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		fprintf( stream, "%s\n", readerTable[ j ] ) ;
		if( first == 1 ) {
			first = 0 ;
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				fprintf( stream, "\"%s\"    ", modalityTable[ i ] ) ;
			}
			fprintf( stream, "\n" ) ;
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				fprintf( stream, "L    ", modalityTable[ i ] ) ;
			}
			fprintf( stream, "\n" ) ;
		}
		for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				fprintf( stream, " %16.5e ", FP[ i ][ j ][ k ] ) ;
			}
			fprintf( stream, " %5d ", k ) ;
			fprintf( stream, "\n" ) ;
		}
		fprintf( stream, "*\n" ) ;
		for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				fprintf( stream, " %16.5e ", TP[ i ][ j ][ k ] ) ;
			}
			fprintf( stream, " %5d ", k + MAX_CASES[ 0 ] ) ;
			fprintf( stream, "\n" ) ;
		}
		fprintf( stream, "*\n" ) ;
	}
	fprintf( stream, "#\n" ) ;

	fclose( stream ) ;

	clear_roc_data() ;

	
	//

	fprintf( report, "\n" ) ;
	fprintf( report, "================================================================================\n" ) ;
	fprintf( report, "\n" ) ;
	fprintf( report, " Equivalent ROC data file in LABMRMC format is saved in file : %s\n", filename ) ;
	fprintf( report, " Do ROC Analysis using DBM MRMC software available at\n" ) ;
	fprintf( report, " Univ of Iowa or Unive of chicago websites\n" ) ;
	fprintf( report, "\n" ) ;

	WriteToConsole( "LABMRMC data file generated successfully.\r\n\r\n" ) ;

	return true ;
}
