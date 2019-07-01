#include "stdafx.h"
#include "../common.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int array_count( int *arr, int len, int cutoff1, int cutoff2 )
{
	int count = 0 ;
	for( int i = 0 ; i < len ; i++ ) {
		if( arr[ i ] > cutoff1 && arr[ i ] <= cutoff2 ) count++ ;
	}
	return count ;
}


int array_count( int **arr, int len1, int len2, int cutoff1, int cutoff2 )
{
	int count = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			if( arr[ i1 ][ i2 ] > cutoff1 && arr[ i1 ][ i2 ] <= cutoff2 ) count++ ;
		}
	}
	return count ;
}


int array_count( int ***arr, int len1, int len2, int len3, int cutoff1, int cutoff2 )
{
	int count = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				if( arr[ i1 ][ i2 ][ i3 ] > cutoff1 && arr[ i1 ][ i2 ][ i3 ] <= cutoff2 ) count++ ;
			}
		}
	}
	return count ;
}


int array_count( int ****arr, int len1, int len2, int len3, int len4, int cutoff1, int cutoff2 )
{
	int count = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				for( int i4 = 0 ; i4 < len4 ; i4++ ) {
					if( arr[ i1 ][ i2 ][ i3 ][ i4 ] > cutoff1 && arr[ i1 ][ i2 ][ i3 ][ i4 ] <= cutoff2 ) count++ ;
				}
			}
		}
	}
	return count ;
}



int array_count( float *arr, int len, float cutoff1, float cutoff2 )
{
	int count = 0 ;
	for( int i = 0 ; i < len ; i++ ) {
		if( arr[ i ] > cutoff1 && arr[ i ] <= cutoff2 ) count++ ;
	}
	return count ;
}


int array_count( float **arr, int len1, int len2, float cutoff1, float cutoff2 )
{
	int count = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			if( arr[ i1 ][ i2 ] > cutoff1 && arr[ i1 ][ i2 ] <= cutoff2 ) count++ ;
		}
	}
	return count ;
}


int array_count( float ***arr, int len1, int len2, int len3, float cutoff1, float cutoff2 )
{
	int count = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				if( arr[ i1 ][ i2 ][ i3 ] > cutoff1 && arr[ i1 ][ i2 ][ i3 ] <= cutoff2 ) count++ ;
			}
		}
	}
	return count ;
}


int array_count( float ****arr, int len1, int len2, int len3, int len4, float cutoff1, float cutoff2 )
{
	int count = 0 ;
	for( int i1 = 0 ; i1 < len1 ; i1++ ) {
		for( int i2 = 0 ; i2 < len2 ; i2++ ) {
			for( int i3 = 0 ; i3 < len3 ; i3++ ) {
				for( int i4 = 0 ; i4 < len4 ; i4++ ) {
					if( arr[ i1 ][ i2 ][ i3 ][ i4 ] > cutoff1 && arr[ i1 ][ i2 ][ i3 ][ i4 ] <= cutoff2 ) count++ ;
				}
			}
		}
	}
	return count ;
}
