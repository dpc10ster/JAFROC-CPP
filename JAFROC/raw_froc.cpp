#include "stdafx.h"
#include "common.h"
#include <math.h>
#include <nr.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif




int max_points_froc ;
int **nlf_max_froc = NULL ;
float ***nlf_raw_froc = NULL ;
float ***llf_raw_froc = NULL ;

int max_points_afroc ;
int **fpf_max_afroc = NULL ;
float ***fpf_raw_afroc = NULL ;
float ***llf_raw_afroc = NULL ;

int max_points_roc ;
int **fpf_max_roc = NULL ;
float ***fpf_raw_roc = NULL ;
float ***tpf_raw_roc = NULL ;

float **nlf_avg_raw_froc = NULL ;
float **llf_avg_raw_froc = NULL ;
float *fpf_avg_raw_afroc = NULL ;
float **llf_avg_raw_afroc = NULL ;
float *fpf_avg_raw_roc = NULL ;
float **tpf_avg_raw_roc = NULL ;



void init_raw_froc( void )
{
	if( nlf_raw_froc != NULL ) clear_raw_froc() ;
	/*
	int nl_max_points = 0, ll_max_points = 0 ;
	nl_max_points = ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) * MAX_NL ;
	ll_max_points = ( MAX_CASES[ 1 ] ) * MAX_LL ;
	if( nl_max_points > ll_max_points ) max_points_froc = nl_max_points ;
	else max_points_froc = ll_max_points ;
	*/
	nlf_raw_froc = new float **[ N_TREATMENTS ] ;
	llf_raw_froc = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		nlf_raw_froc[ i ] = new float *[ MAX_READERS ] ;
		llf_raw_froc[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			nlf_raw_froc[ i ][ j ] = new float[ max_points_froc ] ;
			llf_raw_froc[ i ][ j ] = new float[ max_points_froc ] ;
			for( int k = 0 ; k < max_points_froc ; k++ ) {
				nlf_raw_froc[ i ][ j ][ k ] = UNINITIALIZED ;
				llf_raw_froc[ i ][ j ][ k ] = UNINITIALIZED ;
			}
		}
	}
	nlf_max_froc = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		nlf_max_froc[ i ] = new int[ MAX_READERS ] ;
	}

	nlf_avg_raw_froc = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		nlf_avg_raw_froc[ i ] = new float[ N_AVG_RAW_PTS ] ;
	}
	llf_avg_raw_froc = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		llf_avg_raw_froc[ i ] = new float[ N_AVG_RAW_PTS ] ;
	}
}


void clear_raw_froc( void )
{
	if( nlf_raw_froc == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] nlf_raw_froc[ i ][ j ] ;
			delete[] llf_raw_froc[ i ][ j ] ;
		}
		delete[] nlf_raw_froc[ i ] ;
		delete[] llf_raw_froc[ i ] ;
	}
	delete[] nlf_raw_froc ;
	delete[] llf_raw_froc ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] nlf_max_froc[ i ] ;
	}
	delete[] nlf_max_froc ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] nlf_avg_raw_froc[ i ] ;
	}
	delete[] nlf_avg_raw_froc ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] llf_avg_raw_froc[ i ] ;
	}
	delete[] llf_avg_raw_froc ;

	nlf_raw_froc = NULL ;
	llf_raw_froc = NULL ;
	nlf_max_froc = NULL ;
}



void raw_froc_op_points( void )
{
	int max_points_nl = 0, max_points_ll = 0 ;
	float maxr = -1e15f, minr = 1e15f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_NL ; l++ ) {
					if( NL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) {
						max_points_nl++ ;
						if( NL[ i ][ j ][ k ][ l ] > maxr ) maxr = NL[ i ][ j ][ k ][ l ] ;
						if( NL[ i ][ j ][ k ][ l ] < minr ) minr = NL[ i ][ j ][ k ][ l ] ;
					}
				}
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_LL ; l++ ) {
					if( LL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) {
						max_points_ll++ ;
						if( LL[ i ][ j ][ k ][ l ] > maxr ) maxr = LL[ i ][ j ][ k ][ l ] ;
						if( LL[ i ][ j ][ k ][ l ] < minr ) minr = LL[ i ][ j ][ k ][ l ] ;
					}
				}
			}
		}
	}
	if( max_points_nl > max_points_ll ) max_points_froc = max_points_nl ;
	else max_points_froc = max_points_ll ;

	float eps = ( maxr - minr ) / (float)( max_points_froc * 10 ) ;
	float zeta_max = maxr + 2.0f * eps ;
	float zeta_min = minr - 2.0f * eps ;

	init_raw_froc() ;

	float endpoint_nlf = -1e15 ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int op1 = -1 ;
			float zeta = zeta_max ;
		
			int nl_count_old = 0, ll_count_old = 0 ;

			while( zeta > zeta_min ) {
				int nl_count = 0, ll_count = 0 ;

				for( int k = 0 ; k < MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ; k++ ) {
					for( int l = 0 ; l < MAX_NL ; l++ ) {
						if( NL[ i ][ j ][ k ][ l ] > zeta - eps ) nl_count++ ;
					}
				}
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					for( int l = 0 ; l < MAX_LL ; l++ ) {
						if( LL[ i ][ j ][ k ][ l ] > zeta - eps ) ll_count++ ;
					}
				}
				if( nl_count != nl_count_old || ll_count != ll_count_old ) {
					op1++ ;
					nlf_raw_froc[ i ][ j ][ op1 ] = (float)nl_count ;
					llf_raw_froc[ i ][ j ][ op1 ] = (float)ll_count ;
					nl_count_old = nl_count ;
					ll_count_old = ll_count ;
				}
				zeta = zeta - eps ;
			}
			int *nlf_index = new int[ max_points_froc ] ;
			int *llf_index = new int[ max_points_froc ] ;
			int nlf_count, llf_count ;
			where( nlf_raw_froc[ i ][ j ], max_points_froc, "ne", UNINITIALIZED, nlf_index, &nlf_count ) ;
			for( int k = 0 ; k < nlf_count ; k++ ) {
				nlf_raw_froc[ i ][ j ][ k ] = nlf_raw_froc[ i ][ j ][ nlf_index[ k ] ] / (float)( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ) ;
				if( nlf_raw_froc[ i ][ j ][ k ] > endpoint_nlf ) endpoint_nlf = nlf_raw_froc[ i ][ j ][ k ] ;
			}
//			nlf_raw_froc[ i ][ j ][ nlf_count ] = nlf_raw_froc[ i ][ j ][ nlf_count - 1 ] + 1.0f ;
//			nlf_raw_froc[ i ][ j ][ nlf_count ] = -UNINITIALIZED ;
			where( llf_raw_froc[ i ][ j ], max_points_froc, "ne", UNINITIALIZED, llf_index, &llf_count ) ;
			for( int k = 0 ; k < llf_count ; k++ ) {
				llf_raw_froc[ i ][ j ][ k ] = llf_raw_froc[ i ][ j ][ llf_index[ k ] ] / (float)( total( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
			}
//			llf_raw_froc[ i ][ j ][ llf_count ] = llf_raw_froc[ i ][ j ][ llf_count - 1 ] ;
			delete[] nlf_index ;
			delete[] llf_index ;
			nlf_max_froc[ i ][ j ] = nlf_count ;
		}
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		float max_nlf = -1e15 ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			if( nlf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] - 1 ] > max_nlf ) max_nlf = nlf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] - 1 ] ;
		}
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			nlf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] ] = max_nlf ;
			llf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] ] = llf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] - 1 ] ;
		}
		for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
			nlf_avg_raw_froc[ i ][ pt ] = (float)pt / (float)N_AVG_RAW_PTS * max_nlf ;
		}
		for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
			llf_avg_raw_froc[ i ][ pt ] = 0.0f ;
		}
//	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
				int index = 0 ;
				for( int k = 0 ; k < max_points_froc ; k++ ) {
					if( nlf_raw_froc[ i ][ j ][ k ] != UNINITIALIZED ) {
						if( nlf_raw_froc[ i ][ j ][ k ] <= nlf_avg_raw_froc[ i ][ pt ] ) index = k ;
					}
				}
				float lower_x = nlf_raw_froc[ i ][ j ][ index ] ;
				float upper_x = nlf_raw_froc[ i ][ j ][ index + 1 ] ;
				float lower_y = llf_raw_froc[ i ][ j ][ index ] ;
				float upper_y = llf_raw_froc[ i ][ j ][ index + 1 ] ;
				float slope = 0.0f ;
				if( upper_x != lower_x ) slope = ( upper_y - lower_y ) / ( upper_x - lower_x ) ;
				llf_avg_raw_froc[ i ][ pt ] += lower_y + slope * ( nlf_avg_raw_froc[ i ][ pt ] - lower_x ) ;
			}
		}
//	}
//	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
			llf_avg_raw_froc[ i ][ pt ] /= (float)MAX_READERS ;
		}
	}
}



void init_raw_roc( void )
{
	if( fpf_raw_roc != NULL ) clear_raw_roc() ;

	/*
	int fp_max_points = 0, tp_max_points = 0 ;
	fp_max_points = ( MAX_CASES[ 0 ] ) ;
	tp_max_points = ( MAX_CASES[ 1 ] ) ;
	if( fp_max_points > tp_max_points ) max_points_roc = fp_max_points ;
	else max_points_roc = tp_max_points ;
	*/

	fpf_raw_roc = new float **[ N_TREATMENTS ] ;
	tpf_raw_roc = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fpf_raw_roc[ i ] = new float *[ MAX_READERS ] ;
		tpf_raw_roc[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			fpf_raw_roc[ i ][ j ] = new float[ max_points_roc ] ;
			tpf_raw_roc[ i ][ j ] = new float[ max_points_roc ] ;
			for( int k = 0 ; k < max_points_roc ; k++ ) {
				fpf_raw_roc[ i ][ j ][ k ] = UNINITIALIZED ;
				tpf_raw_roc[ i ][ j ][ k ] = UNINITIALIZED ;
			}
		}
	}

	fpf_max_roc = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fpf_max_roc[ i ] = new int[ MAX_READERS ] ;
	}

	fpf_avg_raw_roc = new float[ N_AVG_RAW_PTS ] ;
	tpf_avg_raw_roc = new float *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		tpf_avg_raw_roc[ i ] = new float[ N_AVG_RAW_PTS ] ;
	}
}


void clear_raw_roc( void )
{
	if( fpf_raw_roc == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] fpf_raw_roc[ i ][ j ] ;
			delete[] tpf_raw_roc[ i ][ j ] ;
		}
		delete[] fpf_raw_roc[ i ] ;
		delete[] tpf_raw_roc[ i ] ;
	}
	delete[] fpf_raw_roc ;
	delete[] tpf_raw_roc ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] fpf_max_roc[ i ] ;
	}
	delete[] fpf_max_roc ;

	delete[] fpf_avg_raw_roc ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] tpf_avg_raw_roc[ i ] ;
	}
	delete[] tpf_avg_raw_roc ;

	fpf_raw_roc = NULL ;
	tpf_raw_roc = NULL ;
}



void raw_roc_op_points( void )
{
	int max_points_fp = 0, max_points_tp = 0 ;
	float maxr = -1e15f, minr = 1e15f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
				float rating = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
				if( rating != UNINITIALIZED ) {
					max_points_fp++ ;
					if( rating > maxr ) maxr = rating ;
					if( rating < minr ) minr = rating ;
				}
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				float nl_rating = maximum( NL[ i ][ j ][ k + MAX_CASES[ 0 ] ], MAX_NL ) ;
				float ll_rating = maximum( LL[ i ][ j ][ k ], MAX_LL ) ;
				float rating = nl_rating ; if( ll_rating > rating ) rating = ll_rating ;
				if( rating != UNINITIALIZED ) {
					max_points_tp++ ;
					if( rating > maxr ) maxr = rating ;
					if( rating < minr ) minr = rating ;
				}
			}
		}
	}
	if( max_points_fp > max_points_tp ) max_points_roc = max_points_fp ;
	else max_points_roc = max_points_tp ;

	float eps = ( maxr - minr ) / (float)( max_points_roc * 10 ) ;
	float zeta_max = maxr + 2.0f * eps ;
	float zeta_min = minr - 2.0f * eps ;

	init_raw_roc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int op1 = -1 ;
			float zeta = zeta_max ;
		
			int fp_count_old = 0, tp_count_old = 0 ;

			while( zeta > zeta_min ) {
				int fp_count = 0, tp_count = 0 ;

				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					float rating = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
					if( rating > zeta - eps ) fp_count++ ;
				}
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					float nl_rating = maximum( NL[ i ][ j ][ k + MAX_CASES[ 0 ] ], MAX_NL ) ;
					float ll_rating = maximum( LL[ i ][ j ][ k ], MAX_LL ) ;
					float rating = nl_rating ; if( ll_rating > rating ) rating = ll_rating ;
					if( rating > zeta - eps ) tp_count++ ;
				}
				if( fp_count != fp_count_old || tp_count != tp_count_old ) {
					op1++ ;
					fpf_raw_roc[ i ][ j ][ op1 ] = (float)fp_count ;
					tpf_raw_roc[ i ][ j ][ op1 ] = (float)tp_count ;
					fp_count_old = fp_count ;
					tp_count_old = tp_count ;
				}
				zeta = zeta - eps ;
			}
			int *fpf_index = new int[ max_points_roc ] ;
			int *tpf_index = new int[ max_points_roc ] ;
			int fpf_count, tpf_count ;
			where( fpf_raw_roc[ i ][ j ], max_points_roc, "ne", UNINITIALIZED, fpf_index, &fpf_count ) ;
			for( int k = 0 ; k < fpf_count ; k++ ) {
				fpf_raw_roc[ i ][ j ][ k ] = fpf_raw_roc[ i ][ j ][ fpf_index[ k ] ] / (float)( MAX_CASES[ 0 ] ) ;
			}
			fpf_raw_roc[ i ][ j ][ fpf_count ] = 1.0f ; fpf_count++ ;
			where( tpf_raw_roc[ i ][ j ], max_points_roc, "ne", UNINITIALIZED, tpf_index, &tpf_count ) ;
			for( int k = 0 ; k < tpf_count ; k++ ) {
				tpf_raw_roc[ i ][ j ][ k ] = tpf_raw_roc[ i ][ j ][ tpf_index[ k ] ] / (float)( MAX_CASES[ 1 ] ) ;
			}
			tpf_raw_roc[ i ][ j ][ tpf_count ] = 1.0f ; tpf_count++ ;
			delete[] fpf_index ;
			delete[] tpf_index ;
			fpf_max_roc[ i ][ j ] = fpf_count ;
		}
	}

	for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
		fpf_avg_raw_roc[ pt ] = (float)pt / (float)N_AVG_RAW_PTS ;
	}
	for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			tpf_avg_raw_roc[ i ][ pt ] = 0.0f ;
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
				int index = 0 ;
				for( int k = 0 ; k < max_points_roc ; k++ ) {
					if( fpf_raw_roc[ i ][ j ][ k ] != UNINITIALIZED ) {
						if( fpf_raw_roc[ i ][ j ][ k ] <= fpf_avg_raw_roc[ pt ] ) index = k ;
					}
				}
				float lower_x = fpf_raw_roc[ i ][ j ][ index ] ; if( lower_x == UNINITIALIZED ) lower_x = 1.0f ;
				float upper_x = fpf_raw_roc[ i ][ j ][ index + 1 ] ; if( upper_x == UNINITIALIZED ) upper_x = 1.0f ;
				float lower_y = tpf_raw_roc[ i ][ j ][ index ] ; if( lower_y == UNINITIALIZED ) lower_y = 1.0f ;
				float upper_y = tpf_raw_roc[ i ][ j ][ index + 1 ] ; if( upper_y == UNINITIALIZED ) upper_y = 1.0f ;
				float slope = 0.0f ;
				if( upper_x != lower_x ) slope = ( upper_y - lower_y ) / ( upper_x - lower_x ) ;
				tpf_avg_raw_roc[ i ][ pt ] += lower_y + slope * ( fpf_avg_raw_roc[ pt ] - lower_x ) ;
			}
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
			tpf_avg_raw_roc[ i ][ pt ] /= (float)MAX_READERS ;
		}
	}

}



void init_raw_afroc( void )
{
	if( fpf_raw_afroc != NULL ) clear_raw_afroc() ;
	/*
	int fp_max_points = 0, ll_max_points = 0 ;
	fp_max_points = ( MAX_CASES[ 0 ] ) ;	// hjy afroc
	ll_max_points = ( MAX_CASES[ 1 ] ) * MAX_LL ;
	if( fp_max_points > ll_max_points ) max_points_afroc = fp_max_points ;
	else max_points_afroc = ll_max_points ;
	*/

	fpf_raw_afroc = new float **[ N_TREATMENTS ] ;
	llf_raw_afroc = new float **[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fpf_raw_afroc[ i ] = new float *[ MAX_READERS ] ;
		llf_raw_afroc[ i ] = new float *[ MAX_READERS ] ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			fpf_raw_afroc[ i ][ j ] = new float[ max_points_afroc ] ;
			llf_raw_afroc[ i ][ j ] = new float[ max_points_afroc ] ;
			for( int k = 0 ; k < max_points_afroc ; k++ ) {
				fpf_raw_afroc[ i ][ j ][ k ] = UNINITIALIZED ;
				llf_raw_afroc[ i ][ j ][ k ] = UNINITIALIZED ;
			}
		}
	}

	fpf_max_afroc = new int *[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fpf_max_afroc[ i ] = new int[ MAX_READERS ] ;
	}

	fpf_avg_raw_afroc = new float[ N_AVG_RAW_PTS ] ;
	llf_avg_raw_afroc = new float *[ N_AVG_RAW_PTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		llf_avg_raw_afroc[ i ] = new float[ N_AVG_RAW_PTS ] ;
	}
}


void clear_raw_afroc( void )
{
	if( fpf_raw_afroc == NULL ) return ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			delete[] fpf_raw_afroc[ i ][ j ] ;
			delete[] llf_raw_afroc[ i ][ j ] ;
		}
		delete[] fpf_raw_afroc[ i ] ;
		delete[] llf_raw_afroc[ i ] ;
	}
	delete[] fpf_raw_afroc ;
	delete[] llf_raw_afroc ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] fpf_max_afroc[ i ] ;
	}
	delete[] fpf_max_afroc ;

	delete[] fpf_avg_raw_afroc ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		delete[] llf_avg_raw_afroc[ i ] ;
	}
	delete[] llf_avg_raw_afroc ;

	fpf_raw_afroc = NULL ;
	llf_raw_afroc = NULL ;
}



void raw_afroc_op_points( void )
{
	int max_points_fp = 0, max_points_ll = 0 ;
	float maxr = -1e15f, minr = 1e15f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {	// hjy afroc
				float rating = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
				if( rating != UNINITIALIZED ) {
					max_points_fp++ ;
					if( rating > maxr ) maxr = rating ;
					if( rating < minr ) minr = rating ;
				}
			}
			for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
				for( int l = 0 ; l < MAX_LL ; l++ ) {
					if( LL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) {
						max_points_ll++ ;
						if( LL[ i ][ j ][ k ][ l ] > maxr ) maxr = LL[ i ][ j ][ k ][ l ] ;
						if( LL[ i ][ j ][ k ][ l ] < minr ) minr = LL[ i ][ j ][ k ][ l ] ;
					}
				}
			}
		}
	}
	if( max_points_fp > max_points_ll ) max_points_afroc = max_points_fp ;
	else max_points_afroc = max_points_ll ;

	float eps = ( maxr - minr ) / (float)( max_points_afroc * 10 ) ;
	float zeta_max = maxr + 2.0f * eps ;
	float zeta_min = minr - 2.0f * eps ;

	init_raw_afroc() ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			int op1 = -1 ;
			float zeta = zeta_max ;
		
			int fp_count_old = 0, ll_count_old = 0 ;

			while( zeta > zeta_min ) {
				int fp_count = 0, ll_count = 0 ;

				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {	// hjy afroc
					float rating = maximum( NL[ i ][ j ][ k ], MAX_NL ) ;
					if( rating > zeta - eps ) fp_count++ ;
				}
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					for( int l = 0 ; l < MAX_LL ; l++ ) {
						if( LL[ i ][ j ][ k ][ l ] > zeta - eps ) ll_count++ ;
					}
				}
				if( fp_count != fp_count_old || ll_count != ll_count_old ) {
					op1++ ;
					fpf_raw_afroc[ i ][ j ][ op1 ] = (float)fp_count ;
					llf_raw_afroc[ i ][ j ][ op1 ] = (float)ll_count ;
					fp_count_old = fp_count ;
					ll_count_old = ll_count ;
				}
				zeta = zeta - eps ;
			}
			int *fpf_index = new int[ max_points_afroc ] ;
			int *llf_index = new int[ max_points_afroc ] ;
			int fpf_count, llf_count ;
			where( fpf_raw_afroc[ i ][ j ], max_points_afroc, "ne", UNINITIALIZED, fpf_index, &fpf_count ) ;
			for( int k = 0 ; k < fpf_count ; k++ ) {
				fpf_raw_afroc[ i ][ j ][ k ] = fpf_raw_afroc[ i ][ j ][ fpf_index[ k ] ] / (float)( MAX_CASES[ 0 ] ) ;	// hjy afroc
			}
			fpf_raw_afroc[ i ][ j ][ fpf_count ] = 1.0f ; fpf_count++ ;
			where( llf_raw_afroc[ i ][ j ], max_points_afroc, "ne", UNINITIALIZED, llf_index, &llf_count ) ;
			for( int k = 0 ; k < llf_count ; k++ ) {
				llf_raw_afroc[ i ][ j ][ k ] = llf_raw_afroc[ i ][ j ][ llf_index[ k ] ] / (float)( total( N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
			}
			llf_raw_afroc[ i ][ j ][ llf_count ] = 1.0f ; llf_count++ ;
			delete[] fpf_index ;
			delete[] llf_index ;
			fpf_max_afroc[ i ][ j ] = fpf_count ;
		}
	}

	for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
		fpf_avg_raw_afroc[ pt ] = (float)pt / (float)N_AVG_RAW_PTS ;
	}
	for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			llf_avg_raw_afroc[ i ][ pt ] = 0.0f ;
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
				int index = 0 ;
				for( int k = 0 ; k < max_points_afroc ; k++ ) {
					if( fpf_raw_afroc[ i ][ j ][ k ] != UNINITIALIZED ) {
						if( fpf_raw_afroc[ i ][ j ][ k ] <= fpf_avg_raw_afroc[ pt ] ) index = k ;
					}
				}
				float lower_x = fpf_raw_afroc[ i ][ j ][ index ] ;
				float upper_x = fpf_raw_afroc[ i ][ j ][ index + 1 ] ;
				float lower_y = llf_raw_afroc[ i ][ j ][ index ] ;
				float upper_y = llf_raw_afroc[ i ][ j ][ index + 1 ] ;
				float slope = 0.0f ;
				if( upper_x != lower_x ) slope = ( upper_y - lower_y ) / ( upper_x - lower_x ) ;
				float xinc = ( fpf_avg_raw_afroc[ pt ] - lower_x ) ;
				if( xinc < 0.0f ) xinc = 0.0f ;
				llf_avg_raw_afroc[ i ][ pt ] += lower_y + slope * xinc ;
			}
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int pt = 0 ; pt < N_AVG_RAW_PTS ; pt++ ) {
			llf_avg_raw_afroc[ i ][ pt ] /= (float)MAX_READERS ;
		}
	}

}
