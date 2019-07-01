#include "stdafx.h"
#include "../common.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



bool print_pseudovalues( FILE *report, char *filename )
{
	FILE *stream ;
	errno_t err = fopen_s( &stream, filename, "wt" ) ;
	if( err != 0 ) {
		fprintf( report, "\n" ) ;
		fprintf( report, "================================================================================\n" ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, " Error generating pseudovalues file \n" ) ;
		fprintf( report, "\n" ) ;

		WriteToConsole( "Error generating pseudovalues file.\r\n\r\n" ) ;

		return false ;
	}

	fprintf( stream, "Reader\tTreatment\tCase\tPV\n" ) ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				fprintf( stream, "R%d\tM%d\tC%d\t%15.10f\n", j+1, i+1, k+1, PseudoValues[ i ][ j ][ k ] ) ;
			}
		}
	}
	fclose( stream ) ;

	//

	fprintf( report, "\n" ) ;
	fprintf( report, "================================================================================\n" ) ;
	fprintf( report, "\n" ) ;
	fprintf( report, " Pseudovalues are saved in file : %s\n", filename ) ;
	fprintf( report, "\n" ) ;

	WriteToConsole( "Pseudovalues file generated successfully.\r\n\r\n" ) ;

	return true ;
}
