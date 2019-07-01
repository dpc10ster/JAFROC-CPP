#include "stdafx.h"
#include "../common.h"
#include <string.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif




bool where( int *arr, int arrlen, char *exp, int val, int *index, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i = 0 ; i < arrlen ; i++ ) {
		switch( stat ) {
			case 0:
				if( arr[ i ] > val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 1:
				if( arr[ i ] >= val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 2:
				if( arr[ i ] == val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 3:
				if( arr[ i ] != val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 4:
				if( arr[ i ] <= val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 5:
				if( arr[ i ] < val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
		}
	}
	*retlen = inc ;
	return true ;
}

bool where( int **arr, int len1, int len2, char *exp, int val, int *index, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			switch( stat ) {
				case 0:
					if( arr[ i1 ][ i2 ] > val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 1:
					if( arr[ i1 ][ i2 ] >= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 2:
					if( arr[ i1 ][ i2 ] == val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 3:
					if( arr[ i1 ][ i2 ] != val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 4:
					if( arr[ i1 ][ i2 ] <= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 5:
					if( arr[ i1 ][ i2 ] < val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
			}
		}
	}
	*retlen = inc ;
	return true ;
}


bool where( int *arr, int arrlen, char *exp, int val, int *index, int *ret, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i = 0 ; i < arrlen ; i++ ) {
		switch( stat ) {
			case 0:
				if( arr[ i ] > val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 1:
				if( arr[ i ] >= val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 2:
				if( arr[ i ] == val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 3:
				if( arr[ i ] != val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 4:
				if( arr[ i ] <= val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 5:
				if( arr[ i ] < val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
		}
	}
	*retlen = inc ;
	return true ;
}

bool where( int **arr, int len1, int len2, char *exp, int val, int *index, int *ret, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			switch( stat ) {
				case 0:
					if( arr[ i1 ][ i2 ] > val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 1:
					if( arr[ i1 ][ i2 ] >= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 2:
					if( arr[ i1 ][ i2 ] == val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 3:
					if( arr[ i1 ][ i2 ] != val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 4:
					if( arr[ i1 ][ i2 ] <= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 5:
					if( arr[ i1 ][ i2 ] < val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
			}
		}
	}
	*retlen = inc ;
	return true ;
}


bool where( float *arr, int arrlen, char *exp, float val, int *index, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i = 0 ; i < arrlen ; i++ ) {
		switch( stat ) {
			case 0:
				if( arr[ i ] > val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 1:
				if( arr[ i ] >= val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 2:
				if( arr[ i ] == val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 3:
				if( arr[ i ] != val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 4:
				if( arr[ i ] <= val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
			case 5:
				if( arr[ i ] < val ) {
					index[ inc ] = i ;
					inc++ ;
				}
				break ;
		}
	}
	*retlen = inc ;
	return true ;
}


bool where( float **arr, int len1, int len2, char *exp, float val, int *index, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			switch( stat ) {
				case 0:
					if( arr[ i1 ][ i2 ] > val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 1:
					if( arr[ i1 ][ i2 ] >= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 2:
					if( arr[ i1 ][ i2 ] == val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 3:
					if( arr[ i1 ][ i2 ] != val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 4:
					if( arr[ i1 ][ i2 ] <= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
				case 5:
					if( arr[ i1 ][ i2 ] < val ) {
						index[ inc ] = i1 * len2 + i2 ;
						inc++ ;
					}
					break ;
			}
		}
	}
	*retlen = inc ;
	return true ;
}


bool where( float *arr, int arrlen, char *exp, float val, int *index, float *ret, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i = 0 ; i < arrlen ; i++ ) {
		switch( stat ) {
			case 0:
				if( arr[ i ] > val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 1:
				if( arr[ i ] >= val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 2:
				if( arr[ i ] == val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 3:
				if( arr[ i ] != val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 4:
				if( arr[ i ] <= val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
			case 5:
				if( arr[ i ] < val ) {
					index[ inc ] = i ;
					ret[ inc ] = arr[ i ] ;
					inc++ ;
				}
				break ;
		}
	}
	*retlen = inc ;
	return true ;
}


bool where( float **arr, int len1, int len2, char *exp, float val, int *index, float *ret, int *retlen )
{
	int inc = 0 ;
	int stat = -1 ;

	if( !strcmp( exp, "gt" ) ) stat = 0 ;
	else if( !strcmp( exp, "ge" ) ) stat = 1 ;
	else if( !strcmp( exp, "eq" ) ) stat = 2 ;
	else if( !strcmp( exp, "ne" ) ) stat = 3 ;
	else if( !strcmp( exp, "le" ) ) stat = 4 ;
	else if( !strcmp( exp, "lt" ) ) stat = 5 ;
	if( stat == -1 ) return false ;

	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			switch( stat ) {
				case 0:
					if( arr[ i1 ][ i2 ] > val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 1:
					if( arr[ i1 ][ i2 ] >= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 2:
					if( arr[ i1 ][ i2 ] == val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 3:
					if( arr[ i1 ][ i2 ] != val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 4:
					if( arr[ i1 ][ i2 ] <= val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
				case 5:
					if( arr[ i1 ][ i2 ] < val ) {
						index[ inc ] = i1 * len2 + i2 ;
						ret[ inc ] = arr[ i1 ][ i2 ] ;
						inc++ ;
					}
					break ;
			}
		}
	}
	*retlen = inc ;
	return true ;
}
