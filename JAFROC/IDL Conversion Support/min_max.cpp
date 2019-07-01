#include "stdafx.h"
#include "../common.h"
#include <math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int minimum( int *arr, int len )
{
	int temp = arr[ 0 ] ;
	for( int i = 0 ; i < len ; i++ ) {
		if( arr[ i ] < temp ) temp = arr[ i ] ;
	}
	return temp ;
}

int minimum( int **arr, int len1, int len2 )
{
	int temp = arr[ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			if( arr[ i1 ][ i2 ] < temp ) temp = arr[ i1 ][ i2 ] ;
		}
	}
	return temp ;
}

int minimum( int ***arr, int len1, int len2, int len3 )
{
	int temp = arr[ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				if( arr[ i1 ][ i2 ][ i3 ] < temp ) temp = arr[ i1 ][ i2 ][ i3 ] ;
			}
		}
	}
	return temp ;
}

int minimum( int ****arr, int len1, int len2, int len3, int len4 )
{
	int temp = arr[ 0 ][ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				for( int i4 = 0 ; i4 < len4 ; i4++ ) {
					if( arr[ i1 ][ i2 ][ i3 ][ i4 ] < temp ) temp = arr[ i1 ][ i2 ][ i3 ][ i4 ] ;
				}
			}
		}
	}
	return temp ;
}

float minimum( float *arr, int len )
{
	float temp = arr[ 0 ] ;
	for( int i = 0 ; i < len ; i++ ) {
		if( arr[ i ] < temp ) temp = arr[ i ] ;
	}
	return temp ;
}

float minimum( float **arr, int len1, int len2 )
{
	float temp = arr[ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			if( arr[ i1 ][ i2 ] < temp ) temp = arr[ i1 ][ i2 ] ;
		}
	}
	return temp ;
}

float minimum( float ***arr, int len1, int len2, int len3 )
{
	float temp = arr[ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				if( arr[ i1 ][ i2 ][ i3 ] < temp ) temp = arr[ i1 ][ i2 ][ i3 ] ;
			}
		}
	}
	return temp ;
}

float minimum( float ****arr, int len1, int len2, int len3, int len4 )
{
	float temp = arr[ 0 ][ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				for( int i4 = 0 ; i4 < len4 ; i4++ ) {
					if( arr[ i1 ][ i2 ][ i3 ][ i4 ] < temp ) temp = arr[ i1 ][ i2 ][ i3 ][ i4 ] ;
				}
			}
		}
	}
	return temp ;
}

int maximum( int *arr, int len )
{
	int temp = arr[ 0 ] ;
	for( int i = 0 ; i < len ; i++ ) {
		if( arr[ i ] > temp ) temp = arr[ i ] ;
	}
	return temp ;
}

int maximum( int **arr, int len1, int len2 )
{
	int temp = arr[ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			if( arr[ i1 ][ i2 ] > temp ) temp = arr[ i1 ][ i2 ] ;
		}
	}
	return temp ;
}

int maximum( int ***arr, int len1, int len2, int len3 )
{
	int temp = arr[ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				if( arr[ i1 ][ i2 ][ i3 ] > temp ) temp = arr[ i1 ][ i2 ][ i3 ] ;
			}
		}
	}
	return temp ;
}

int maximum( int ****arr, int len1, int len2, int len3, int len4 )
{
	int temp = arr[ 0 ][ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				for( int i4 = 0 ; i4 < len4 ; i4++ ) {
					if( arr[ i1 ][ i2 ][ i3 ][ i4 ] > temp ) temp = arr[ i1 ][ i2 ][ i3 ][ i4 ] ;
				}
			}
		}
	}
	return temp ;
}

float maximum( float *arr, int len )
{
	float temp = arr[ 0 ] ;
	for( int i = 0 ; i < len ; i++ ) {
		if( arr[ i ] > temp ) temp = arr[ i ] ;
	}
	return temp ;
}

float maximum( float **arr, int len1, int len2 )
{
	float temp = arr[ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			if( arr[ i1 ][ i2 ] > temp ) temp = arr[ i1 ][ i2 ] ;
		}
	}
	return temp ;
}

float maximum( float ***arr, int len1, int len2, int len3 )
{
	float temp = arr[ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				if( arr[ i1 ][ i2 ][ i3 ] > temp ) temp = arr[ i1 ][ i2 ][ i3 ] ;
			}
		}
	}
	return temp ;
}

float maximum( float ****arr, int len1, int len2, int len3, int len4 )
{
	float temp = arr[ 0 ][ 0 ][ 0 ][ 0 ] ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				for( int i4 = 0 ; i4 < len4 ; i4++ ) {
					if( arr[ i1 ][ i2 ][ i3 ][ i4 ] > temp ) temp = arr[ i1 ][ i2 ][ i3 ][ i4 ] ;
				}
			}
		}
	}
	return temp ;
}


float mean( int *arr, int len )
{
	int temp = 0 ;
	for( int i = 0 ; i < len ; i++ ) {
		temp += arr[ i ] ;
	}
	return (float)temp / (float)len ;
}

float mean( int **arr, int len1, int len2 )
{
	int temp = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			temp += arr[ i1 ][ i2 ] ;
		}
	}
	return (float)temp / (float)len1 / (float)len2 ;
}

float mean( float *arr, int len )
{
	float temp = 0.0f ;
	for( int i = 0 ; i < len ; i++ ) {
		temp += arr[ i ] ;
	}
	return temp / (float)len ;
}

float mean( float **arr, int len1, int len2 )
{
	float temp = 0.0f ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			temp += arr[ i1 ][ i2 ] ;
		}
	}
	return temp / (float)len1 / (float)len2 ;
}


int total( int *arr, int len )
{
	int temp = 0 ;
	for( int i = 0 ; i < len ; i++ ) {
		temp += arr[ i ] ;
	}
	return temp ;
}

int total( int **arr, int len1, int len2 )
{
	int temp = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			temp += arr[ i1 ][ i2 ] ;
		}
	}
	return temp ;
}

int total( int ***arr, int len1, int len2, int len3 )
{
	int temp = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				temp += arr[ i1 ][ i2 ][ i3 ] ;
			}
		}
	}
	return temp ;
}

float total( float *arr, int len )
{
	float temp = 0.0f ;
	for( int i = 0 ; i < len ; i++ ) {
		temp += arr[ i ] ;
	}
	return temp ;
}

float total( float **arr, int len1, int len2 )
{
	float temp = 0.0f ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			temp += arr[ i1 ][ i2 ] ;
		}
	}
	return temp ;
}

float total( float ***arr, int len1, int len2, int len3 )
{
	float temp = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				temp += arr[ i1 ][ i2 ][ i3 ] ;
			}
		}
	}
	return temp ;
}


float stdev( int *arr, int len )
{
	float m = mean( arr, len ) ;
	float temp = 0 ;
	for( int i = 0 ; i < len ; i++ ) {
		temp += ( (float)arr[ i ] - m ) * ( (float)arr[ i ] - m ) ;
	}
	return (float)sqrt( temp / (float)( len - 1 ) ) ;
}

float stdev( float *arr, int len )
{
	float m = mean( arr, len ) ;
	float temp = 0.0f ;
	for( int i = 0 ; i < len ; i++ ) {
		temp += ( arr[ i ] - m ) * ( arr[ i ] - m ) ;
	}
	return (float)sqrt( temp / (float)( len - 1 ) ) ;
}
