#include "stdafx.h"
#include "../common.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



float ***FP = NULL, ***TP = NULL ;



void init_roc_data( void )
{
	if( FP != NULL ) clear_roc_data() ;

	FP = new float **[ N_TREATMENTS ] ;
	TP = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		FP[ i ] = new float *[ MAX_READERS ] ;
		TP[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			if( MAX_CASES[ 0 ] != 0 )
				FP[ i ][ j ] = new float[ MAX_CASES[ 0 ] ] ;
			else
				FP[ i ][ j ] = new float[ MAX_CASES[ 1 ] ] ;
			TP[ i ][ j ] = new float[ MAX_CASES[ 1 ] ] ;
		}
	}
}

void clear_roc_data( void )
{
	if( FP == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] FP[ i ][ j ] ;
			delete[] TP[ i ][ j ] ;
		}
		delete[] FP[ i ] ;
		delete[] TP[ i ] ;
	}
	delete[] FP ;
	delete[] TP ;

	FP = NULL ;
	TP = NULL ;
}



void convert_to_afroc( void )
{

	init_roc_data() ;

	if( MAX_CASES[ 0 ] != 0 ) {	// normal cases exist
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					FP[ i ][ j ][ k ] = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
				}
			}
		}
	} else {	// no normal cases
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					FP[ i ][ j ][ k ] = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
				}
			}
		}
	}
}


void convert_to_roc( void )
{

	init_roc_data() ;

	if( MAX_CASES[ 0 ] != 0 ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					FP[ i ][ j ][ k ] = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
				}
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					float nl = maximum( NL[ i ][ j ][ k + MAX_CASES[ 0 ] ], MAX_NL ) ;
					float ll = maximum( LL[ i ][ j ][ k ], MAX_LL ) ;
					if( nl > ll ) TP[ i ][ j ][ k ] = nl ;
					else TP[ i ][ j ][ k ] = ll ;
				}
			}
		}
	} else {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					float nl = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
					float ll = maximum( LL[ i ][ j ][ k ], MAX_LL ) ;
					FP[ i ][ j ][ k ] = nl ;
					if( nl > ll ) TP[ i ][ j ][ k ] = nl ;
					else TP[ i ][ j ][ k ] = ll ;
				}
			}
		}
	}
}
