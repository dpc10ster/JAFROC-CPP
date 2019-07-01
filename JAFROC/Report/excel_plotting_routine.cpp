#include "stdafx.h"
#include "../common.h"
#include <stdio.h>
#include <math.h>
#include <nr.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define N_TAPS	10


float **avg_parameters_arr ;



/////////////////////////////////////////////////////////////////////
// ROC

void CommonPrintModule_ROC( FILE *plots, int modality, int reader )
{
	float mu_roc, lambda_roc, nu_roc ;
	if( reader == -1 ) {
		mu_roc = avg_parameters_arr[ modality ][ 2 ] ;
		lambda_roc = avg_parameters_arr[ modality ][ 0 ] ;
		nu_roc = avg_parameters_arr[ modality ][ 3 ] ;
	} else {
		mu_roc = roc_fit_parms[ modality ][ reader ][ 2 ] ;
		lambda_roc = roc_fit_parms[ modality ][ reader ][ 0 ] ;
		nu_roc = roc_fit_parms[ modality ][ reader ][ 3 ] ;
	}

	float delta = 1e15f ;
	float midzeta = -2.0f ;
	float deltazeta = 0.5f ;

	while( fabs( delta ) > 1e-2 ) {
		midzeta = midzeta + deltazeta ;
		float xx = fpf_poi( midzeta, lambda_roc ) ;
		float yy = tpf_poi( midzeta, mu_roc, lambda_roc, nu_roc ) ;
		delta = sqrt( xx * xx + yy * yy ) - 
			sqrt( pow( xx - fpf_poi( -20.0f, lambda_roc ), 2.0f ) + pow( yy - tpf_poi( -20.0f, mu_roc, lambda_roc, nu_roc ), 2.0f ) ) ;
		if( delta > 0.0f && deltazeta < 0.0f ) deltazeta = -deltazeta / 2.0f ;
		if( delta <= 0.0f && deltazeta >= 0.0f ) deltazeta = -deltazeta / 2.0f ;
	}
	float mid_x = fpf_poi( midzeta, lambda_roc ) ;
	float mid_y = tpf_poi( midzeta, mu_roc, lambda_roc, nu_roc ) ;
	float taps_y_roc[ N_TAPS + 1 ], taps_x_roc[ N_TAPS + 1 ] ;
	for( int i = 0 ; i < N_TAPS ; i++ ) {
		taps_y_roc[ i ] = (float)i * mid_y / (float)N_TAPS ;
	}
	for( int i = 0 ; i <= N_TAPS ; i++ ) {
		taps_x_roc[ i ] = (float)i * ( fpf_poi( -20.0f, lambda_roc ) - mid_x ) / (float)N_TAPS + mid_x ;
	}

	for( int k = 0 ; k < N_TAPS ; k++ ) {
		float zeta_roc ;
		if( k == 0 ) 
			zeta_roc = 20.0f ;
		else 
			zeta_roc = inv_tpf_poi( taps_y_roc[ k ], mu_roc, lambda_roc, nu_roc ) ;
		float x1 = fpf_poi( zeta_roc, lambda_roc ) ;
		float y1 = tpf_poi( zeta_roc, mu_roc, lambda_roc, nu_roc ) ;
		fprintf( plots, "   %16.4f    %16.4f\n", x1, y1 ) ;
	}
	for( int k = 0 ; k <= N_TAPS ; k++ ) {
		float zeta_roc ;
		if( k == N_TAPS ) 
			zeta_roc = -20.0f ;
		else
			zeta_roc = inv_fpf_poi( taps_x_roc[ k ], lambda_roc ) ;
		float x1 = fpf_poi( zeta_roc, lambda_roc ) ;
		float y1 = tpf_poi( zeta_roc, mu_roc, lambda_roc, nu_roc ) ;
		fprintf( plots, "   %16.4f    %16.4f\n", x1, y1 ) ;
	}
	fprintf( plots, "\n" ) ;
}


void excel_plotting_routine_roc( char *filename )
{
	if( total( roc_fit_errors, N_TREATMENTS, MAX_READERS ) != 0 ) {
		WriteToConsole( "Parameters could not be estimated for some treatment-reader combinations.\r\n" ) ;
		WriteToConsole( "ROC curves will not be shown for them.\r\n\r\n" ) ;
		return ;
	}

	FILE *stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		WriteToConsole( "ROC PLOT: Error opening file.\r\n\r\n" ) ;
		return ;
	}

	fprintf( stream, "SEARCH MODEL PARAMETRIC PLOTS\n" ) ;
	fprintf( stream, "PLOTS SHOWN :\n" ) ;
	fprintf( stream, "ROC\n" ) ;

	int *error_printed = new int[ MAX_READERS ] ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) error_printed[ j ] = 0 ;

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( roc_fit_errors[ i ][ j ] != 0 ) error_printed[ j ] = 1 ;
		}
	}

	fprintf( stream, "NUMBER OF READERS:\n" ) ;
	fprintf( stream, "%d\n", MAX_READERS - total( error_printed, MAX_READERS ) ) ;
	fprintf( stream, "NUMBER OF MODALITIES:\n" ) ;
	fprintf( stream, "%d\n", N_TREATMENTS ) ;
	fprintf( stream, "============================================================\n" ) ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			if( error_printed[ j ] == 1 ) continue ;
			fprintf( stream, "READER:\n" ) ;
			fprintf( stream, "%s\n", readerTable[ j ] ) ;
			fprintf( stream, "MODALITY:\n" ) ;
			fprintf( stream, "%s\n", modalityTable[ i ] ) ;
			fprintf( stream, "SEARCH MODEL PARAMETER VALUES\n" ) ;
			fprintf( stream, "MU = %g, LAMBDA = %g, NU = %g\n"
				, roc_fit_parms[ i ][ j ][ 2 ], roc_fit_parms[ i ][ j ][ 0 ], roc_fit_parms[ i ][ j ][ 3 ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			if( maximum( FPF_BIN_ROC[ i ][ j ], MAX_CTFFS ) == UNINITIALIZED ) {
				fprintf( stream, "0\n" ) ;		// keep this, this informs XLS script data not exist
			} else if( maximum( TPF_BIN_ROC[ i ][ j ], MAX_CTFFS ) == UNINITIALIZED ) {
				fprintf( stream, "0\n" ) ;
			} else {
				int c1 = number_of_bins_ROC[ i ][ j ] ;
				fprintf( stream, "%d\n", c1 ) ;
				fprintf( stream, "        FPF                 TPF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				for( int k = 0 ; k < c1 ; k++ ) {
					fprintf( stream, "   %16.4f    %16.4f\n", FPF_BIN_ROC[ i ][ j ][ k ], TPF_BIN_ROC[ i ][ j ][ k ] ) ;
				}
			}
			fprintf( stream, "\n" ) ;
			fprintf( stream, "Search Model fitted curves\n" ) ;
			if( roc_fit_errors[ i ][ j ] != 0 ) {
				fprintf( stream, "0\n" ) ;		// keep this, this informs XLS script data not exist
			} else {
				fprintf( stream, "%d\n", N_TAPS * 2 ) ;
				fprintf( stream, "            FPF                 TPF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				CommonPrintModule_ROC( stream, i, j ) ;
			}
		}
	}

	delete[] error_printed ;

	if( MAX_READERS > 1 ) {
		avg_parameters_arr = new float *[ N_TREATMENTS ] ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) avg_parameters_arr[ i ] = new float[ 5 ] ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			float *parms_temp0 = new float[ MAX_READERS ] ;
			float *parms_temp1 = new float[ MAX_READERS ] ;
			float *parms_temp2 = new float[ MAX_READERS ] ;
			float *parms_temp3 = new float[ MAX_READERS ] ;
			float *parms_temp4 = new float[ MAX_READERS ] ;
			int c1 = 0 ;
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( roc_fit_errors[ i ][ j ] == 0 ) {
					parms_temp0[ c1 ] = roc_fit_parms[ i ][ j ][ 0 ] ;
					parms_temp1[ c1 ] = roc_fit_parms[ i ][ j ][ 1 ] ;
					parms_temp2[ c1 ] = roc_fit_parms[ i ][ j ][ 2 ] ;
					parms_temp3[ c1 ] = roc_fit_parms[ i ][ j ][ 3 ] ;
					parms_temp4[ c1 ] = roc_fit_parms[ i ][ j ][ 4 ] ;
					c1++ ;
				}
			}
			avg_parameters_arr[ i ][ 0 ] = mean( parms_temp0, c1 ) ;	
			avg_parameters_arr[ i ][ 1 ] = mean( parms_temp1, c1 ) ;
			avg_parameters_arr[ i ][ 2 ] = mean( parms_temp2, c1 ) ;
			avg_parameters_arr[ i ][ 3 ] = mean( parms_temp3, c1 ) ;
			avg_parameters_arr[ i ][ 4 ] = mean( parms_temp4, c1 ) ;

			delete[] parms_temp0 ;
			delete[] parms_temp1 ;
			delete[] parms_temp2 ;
			delete[] parms_temp3 ;
			delete[] parms_temp4 ;
		}
		fprintf( stream, " Average reader curves for each modality\n" ) ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Modality:\n" ) ;
			fprintf( stream, "%s\n", modalityTable[ i ] ) ;
			if( avg_parameters_arr[ i ][ 0 ] == 0 ) {
				fprintf( stream, "0\n" ) ;
			} else {
				fprintf( stream, "%d\n", N_TAPS * 2 ) ;
				fprintf( stream, "            FPF                 TPF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				CommonPrintModule_ROC( stream, i, -1 ) ;
			}
		}

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) delete[] avg_parameters_arr[ i ] ;
		delete[] avg_parameters_arr ;
	}
	fclose( stream ) ;
}



/////////////////////////////////////////////////////////////////////
// AFROC

void CommonPrintModule_AFROC( FILE *plots, int modality, int reader )
{
	float mu_afroc, lambda_afroc, nu_afroc ;
	if( reader == -1 ) {
		mu_afroc = avg_parameters_arr[ modality ][ 2 ] ;
		lambda_afroc = avg_parameters_arr[ modality ][ 0 ] ;
		nu_afroc = avg_parameters_arr[ modality ][ 3 ] ;
	} else {
		mu_afroc = afroc_fit_parms[ modality ][ reader ][ 2 ] ;
		lambda_afroc = afroc_fit_parms[ modality ][ reader ][ 0 ] ;
		nu_afroc = afroc_fit_parms[ modality ][ reader ][ 3 ] ;
	}

	float delta = 1e15f ;
	float midzeta = -2.0f ;
	float deltazeta = 0.5f ;

	while( fabs( delta ) > 1e-2 ) {
		midzeta = midzeta + deltazeta ;
		float xx = fpf_poi( midzeta, lambda_afroc ) ;
		float yy = llf_poi( midzeta, mu_afroc, nu_afroc ) ;
		delta = sqrt( xx * xx + yy * yy ) - 
			sqrt( pow( xx - fpf_poi( -20.0f, lambda_afroc ), 2.0f ) + pow( yy - llf_poi( -20.0f, mu_afroc, nu_afroc ), 2.0f ) ) ;
		if( delta > 0.0f && deltazeta < 0.0f ) deltazeta = -deltazeta / 2.0f ;
		if( delta <= 0.0f && deltazeta >= 0.0f ) deltazeta = -deltazeta / 2.0f ;
	}
	float mid_x = fpf_poi( midzeta, lambda_afroc ) ;
	float mid_y = llf_poi( midzeta, mu_afroc, nu_afroc ) ;
	float taps_y_afroc[ N_TAPS + 1 ], taps_x_afroc[ N_TAPS + 1 ] ;
	for( int i = 0 ; i < N_TAPS ; i++ ) {
		taps_y_afroc[ i ] = (float)i * mid_y / (float)N_TAPS ;
	}
	for( int i = 0 ; i <= N_TAPS ; i++ ) {
		taps_x_afroc[ i ] = (float)i * ( fpf_poi( -20.0f, lambda_afroc ) - mid_x ) / (float)N_TAPS + mid_x ;
	}

	for( int k = 0 ; k < N_TAPS ; k++ ) {
		float zeta_afroc ;
		if( k == 0 ) 
			zeta_afroc = 20.0f ;
		else 
			zeta_afroc = inv_llf_poi( taps_y_afroc[ k ], mu_afroc, nu_afroc ) ;
		float x1 = fpf_poi( zeta_afroc, lambda_afroc ) ;
		float y1 = llf_poi( zeta_afroc, mu_afroc, nu_afroc ) ;
		fprintf( plots, "   %16.4f    %16.4f\n", x1, y1 ) ;
	}
	for( int k = 0 ; k <= N_TAPS ; k++ ) {
		float zeta_afroc ;
		if( k == N_TAPS ) 
			zeta_afroc = -20.0f ;
		else
			zeta_afroc = inv_fpf_poi( taps_x_afroc[ k ], lambda_afroc ) ;
		float x1 = fpf_poi( zeta_afroc, lambda_afroc ) ;
		float y1 = llf_poi( zeta_afroc, mu_afroc, nu_afroc ) ;
		fprintf( plots, "   %16.4f    %16.4f\n", x1, y1 ) ;
	}
	fprintf( plots, "\n" ) ;
}



void excel_plotting_routine_afroc( char *filename )
{
	if( total( afroc_fit_errors, N_TREATMENTS, MAX_READERS ) != 0 ) {
		WriteToConsole( "Parameters could not be estimated for some treatment-reader combinations.\r\n" ) ;
		WriteToConsole( "AFROC curves will not be shown for them.\r\n\r\n" ) ;
		return ;
	}

	FILE *stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		WriteToConsole( "AFROC PLOT: Error opening file.\r\n\r\n" ) ;
		return ;
	}

	fprintf( stream, "SEARCH MODEL PARAMETRIC PLOTS\n" ) ;
	fprintf( stream, "PLOTS SHOWN :\n" ) ;
	fprintf( stream, "AFROC\n" ) ;

	int *error_printed = new int[ MAX_READERS ] ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) error_printed[ j ] = 0 ;

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( afroc_fit_errors[ i ][ j ] != 0 ) error_printed[ j ] = 1 ;
		}
	}

	fprintf( stream, "NUMBER OF READERS:\n" ) ;
	fprintf( stream, "%d\n", MAX_READERS - total( error_printed, MAX_READERS ) ) ;
	fprintf( stream, "NUMBER OF MODALITIES:\n" ) ;
	fprintf( stream, "%d\n", N_TREATMENTS ) ;
	fprintf( stream, "============================================================\n" ) ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			if( error_printed[ j ] == 1 ) continue ;
			fprintf( stream, "READER:\n" ) ;
			fprintf( stream, "%s\n", readerTable[ j ] ) ;
			fprintf( stream, "MODALITY:\n" ) ;
			fprintf( stream, "%s\n", modalityTable[ i ] ) ;
			fprintf( stream, "SEARCH MODEL PARAMETER VALUES\n" ) ;
			fprintf( stream, "MU = %g, LAMBDA = %g, NU = %g\n"
				, afroc_fit_parms[ i ][ j ][ 2 ], afroc_fit_parms[ i ][ j ][ 0 ], afroc_fit_parms[ i ][ j ][ 3 ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			if( maximum( FPF_BIN_AFROC[ i ][ j ], MAX_CTFFS ) == UNINITIALIZED ) {
				fprintf( stream, "0\n" ) ;		// keep this, this informs XLS script data not exist
			} else if( maximum( LLF_BIN_AFROC[ i ][ j ], MAX_CTFFS ) == UNINITIALIZED ) {
				fprintf( stream, "0\n" ) ;
			} else {
//				int c1 = 0 ; for( int k = 0 ; k < MAX_CTFFS ; k++ ) if( FPF_BIN_AFROC[ i ][ j ][ k ] != UNINITIALIZED ) c1++ ;
				int c1 = number_of_bins_AFROC[ i ][ j ] ;
				fprintf( stream, "%d\n", c1 ) ;
				fprintf( stream, "        FPF                 LLF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				for( int k = 0 ; k < c1 ; k++ ) {
					fprintf( stream, "   %16.4f    %16.4f\n", FPF_BIN_AFROC[ i ][ j ][ k ], LLF_BIN_AFROC[ i ][ j ][ k ] ) ;
				}
			}
			fprintf( stream, "\n" ) ;
			fprintf( stream, "Search Model fitted curves\n" ) ;
			if( afroc_fit_errors[ i ][ j ] != 0 ) {
				fprintf( stream, "0\n" ) ;		// keep this, this informs XLS script data not exist
			} else {
				fprintf( stream, "%d\n", N_TAPS * 2 ) ;
				fprintf( stream, "            FPF                 LLF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				CommonPrintModule_AFROC( stream, i, j ) ;
			}
		}
	}

	delete[] error_printed ;

	if( MAX_READERS > 1 ) {
		avg_parameters_arr = new float *[ N_TREATMENTS ] ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) avg_parameters_arr[ i ] = new float[ 5 ] ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			float *parms_temp0 = new float[ MAX_READERS ] ;
			float *parms_temp1 = new float[ MAX_READERS ] ;
			float *parms_temp2 = new float[ MAX_READERS ] ;
			float *parms_temp3 = new float[ MAX_READERS ] ;
			float *parms_temp4 = new float[ MAX_READERS ] ;
			int c1 = 0 ;
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( afroc_fit_errors[ i ][ j ] == 0 ) {
					parms_temp0[ c1 ] = afroc_fit_parms[ i ][ j ][ 0 ] ;
					parms_temp1[ c1 ] = afroc_fit_parms[ i ][ j ][ 1 ] ;
					parms_temp2[ c1 ] = afroc_fit_parms[ i ][ j ][ 2 ] ;
					parms_temp3[ c1 ] = afroc_fit_parms[ i ][ j ][ 3 ] ;
					parms_temp4[ c1 ] = afroc_fit_parms[ i ][ j ][ 4 ] ;
					c1++ ;
				}
			}
			avg_parameters_arr[ i ][ 0 ] = mean( parms_temp0, c1 ) ;	
			avg_parameters_arr[ i ][ 1 ] = mean( parms_temp1, c1 ) ;
			avg_parameters_arr[ i ][ 2 ] = mean( parms_temp2, c1 ) ;
			avg_parameters_arr[ i ][ 3 ] = mean( parms_temp3, c1 ) ;
			avg_parameters_arr[ i ][ 4 ] = mean( parms_temp4, c1 ) ;

			delete[] parms_temp0 ;
			delete[] parms_temp1 ;
			delete[] parms_temp2 ;
			delete[] parms_temp3 ;
			delete[] parms_temp4 ;
		}
		fprintf( stream, " Average reader curves for each modality\n" ) ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Modality:\n" ) ;
			fprintf( stream, "%s\n", modalityTable[ i ] ) ;
			if( avg_parameters_arr[ i ][ 0 ] == 0 ) {
				fprintf( stream, "0\n" ) ;
			} else {
				fprintf( stream, "%d\n", N_TAPS * 2 ) ;
				fprintf( stream, "            FPF                 LLF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				CommonPrintModule_AFROC( stream, i, -1 ) ;
			}
		}

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) delete[] avg_parameters_arr[ i ] ;
		delete[] avg_parameters_arr ;
	}
	fclose( stream ) ;
}



/////////////////////////////////////////////////////////////////////
// FROC

void CommonPrintModule_FROC( FILE *plots, int modality, int reader, float max_x_FROC )
{
	float mu_froc, lambda_froc, nu_froc ;
	if( reader == -1 ) {
		mu_froc = avg_parameters_arr[ modality ][ 2 ] ;
		lambda_froc = avg_parameters_arr[ modality ][ 0 ] ;
		nu_froc = avg_parameters_arr[ modality ][ 3 ] ;
	} else {
		mu_froc = froc_fit_parms[ modality ][ reader ][ 2 ] ;
		lambda_froc = froc_fit_parms[ modality ][ reader ][ 0 ] ;
		nu_froc = froc_fit_parms[ modality ][ reader ][ 3 ] ;
	}

	float delta = 1e15f ;
	float midzeta = -2.0f ;
	float deltazeta = 0.5f ;

	float zeta_min ;
	if( max_x_FROC > lambda_froc ) zeta_min = -20.0f ;
	else zeta_min = inv_nlf_poi( max_x_FROC, lambda_froc ) ;

	while( fabs( delta ) > 1e-2 ) {
		midzeta = midzeta + deltazeta ;
		float xx = nlf_poi( midzeta, lambda_froc ) ;
		float yy = llf_poi( midzeta, mu_froc, nu_froc ) ;
		if( lambda_froc > max_x_FROC ) 
			delta = sqrt( xx * xx + yy * yy ) - 
				sqrt( pow( xx - max_x_FROC, 2.0f ) + pow( yy - llf_poi( -20.0f, mu_froc, nu_froc ), 2.0f ) ) ;
		else
			delta = sqrt( xx * xx + yy * yy ) - 
				sqrt( pow( xx - lambda_froc, 2.0f ) + pow( yy - llf_poi( -20.0f, mu_froc, nu_froc ), 2.0f ) ) ;
		if( delta > 0.0f && deltazeta < 0.0f ) deltazeta = -deltazeta / 2.0f ;
		if( delta <= 0.0f && deltazeta >= 0.0f ) deltazeta = -deltazeta / 2.0f ;
	}
	float mid_x = nlf_poi( midzeta, lambda_froc ) ;
	float mid_y = llf_poi( inv_nlf_poi( mid_x, lambda_froc ), mu_froc, nu_froc ) ;
	float taps_y_froc[ N_TAPS + 1 ], taps_x_froc[ N_TAPS + 1 ] ;
	for( int i = 0 ; i < N_TAPS ; i++ ) {
		taps_y_froc[ i ] = (float)i * mid_y / (float)N_TAPS ;
	}
	for( int i = 0 ; i <= N_TAPS ; i++ ) {
		taps_x_froc[ i ] = (float)i * ( max_x_FROC - mid_x ) / (float)N_TAPS + mid_x ;
	}

	for( int k = 0 ; k < N_TAPS ; k++ ) {
		float zeta_froc ;
		if( k == 0 ) 
			zeta_froc = 20.0f ;
		else 
			zeta_froc = inv_llf_poi( taps_y_froc[ k ], mu_froc, nu_froc ) ;
		float x1 = nlf_poi( zeta_froc, lambda_froc ) ;
		float y1 = llf_poi( zeta_froc, mu_froc, nu_froc ) ;
		fprintf( plots, "   %16.4f    %16.4f\n", x1, y1 ) ;
	}
	for( int k = 0 ; k <= N_TAPS ; k++ ) {
		float zeta_froc ;
		if( k == N_TAPS ) {
			if( taps_x_froc[ N_TAPS ] >= lambda_froc ) zeta_froc = -20.0f ;
			else zeta_froc = inv_nlf_poi( taps_x_froc[ N_TAPS ], lambda_froc ) ;
		} else {
			if( taps_x_froc[ k ] >= lambda_froc ) zeta_froc = -20.0f ;
			else zeta_froc = inv_nlf_poi( taps_x_froc[ k ], lambda_froc ) ;
		}
		float x1 = nlf_poi( zeta_froc, lambda_froc ) ;
		float y1 = llf_poi( zeta_froc, mu_froc, nu_froc ) ;
		fprintf( plots, "   %16.4f    %16.4f\n", x1, y1 ) ;
	}
	fprintf( plots, "\n" ) ;
}



void excel_plotting_routine_froc( char *filename )
{
	if( total( froc_fit_errors, N_TREATMENTS, MAX_READERS ) != 0 ) {
		WriteToConsole( "Parameters could not be estimated for some treatment-reader combinations.\r\n" ) ;
		WriteToConsole( "FROC curves will not be shown for them.\r\n\r\n" ) ;
		return ;
	}

	FILE *stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		WriteToConsole( "FROC PLOT: Error opening file.\r\n\r\n" ) ;
		return ;
	}

	fprintf( stream, "SEARCH MODEL PARAMETRIC PLOTS\n" ) ;
	fprintf( stream, "PLOTS SHOWN :\n" ) ;
	fprintf( stream, "FROC\n" ) ;

	int *error_printed = new int[ MAX_READERS ] ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) error_printed[ j ] = 0 ;

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( froc_fit_errors[ i ][ j ] != 0 ) error_printed[ j ] = 1 ;
		}
	}
	
	float max_x_FROC = UNINITIALIZED ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
//			for( int k = 0 ; k < MAX_CTFFS ; k++ ) {
			for( int k = 0 ; k < number_of_bins_FROC[ i ][ j ] ; k++ ) {
				if( NLF_BIN_FROC[ i ][ j ][ k ] > max_x_FROC ) max_x_FROC = NLF_BIN_FROC[ i ][ j ][ k ] ;
			}
		}
	}
	max_x_FROC = (float)( (int)( max_x_FROC * 100.0f ) ) / 100.0f + 0.2f ;

	fprintf( stream, "NUMBER OF READERS:\n" ) ;
	fprintf( stream, "%d\n", MAX_READERS - total( error_printed, MAX_READERS ) ) ;
	fprintf( stream, "NUMBER OF MODALITIES:\n" ) ;
	fprintf( stream, "%d\n", N_TREATMENTS ) ;
	fprintf( stream, "MAXIMUM NLF:\n" ) ;
	fprintf( stream, "%f\n", max_x_FROC ) ;

	fprintf( stream, "============================================================\n" ) ;

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			if( error_printed[ j ] == 1 ) continue ;
			fprintf( stream, "READER:\n" ) ;
			fprintf( stream, "%s\n", readerTable[ j ] ) ;
			fprintf( stream, "MODALITY:\n" ) ;
			fprintf( stream, "%s\n", modalityTable[ i ] ) ;
			fprintf( stream, "SEARCH MODEL PARAMETER VALUES\n" ) ;
			fprintf( stream, "MU = %g, LAMBDA = %g, NU = %g\n"
				, froc_fit_parms[ i ][ j ][ 2 ], froc_fit_parms[ i ][ j ][ 0 ], froc_fit_parms[ i ][ j ][ 3 ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			if( maximum( NLF_BIN_FROC[ i ][ j ], MAX_CTFFS ) == UNINITIALIZED ) {
				fprintf( stream, "0\n" ) ;		// keep this, this informs XLS script data not exist
			} else if( maximum( LLF_BIN_FROC[ i ][ j ], MAX_CTFFS ) == UNINITIALIZED ) {
				fprintf( stream, "0\n" ) ;
			} else {
				int c1 = number_of_bins_FROC[ i ][ j ] ;
				fprintf( stream, "%d\n", c1 ) ;
				fprintf( stream, "        NLF                 LLF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				for( int k = 0 ; k < c1 ; k++ ) {
					fprintf( stream, "   %16.4f    %16.4f\n", NLF_BIN_FROC[ i ][ j ][ k ], LLF_BIN_FROC[ i ][ j ][ k ] ) ;
				}
			}
			fprintf( stream, "\n" ) ;
			fprintf( stream, "Search Model fitted curves\n" ) ;
			if( froc_fit_errors[ i ][ j ] != 0 ) {
				fprintf( stream, "0\n" ) ;		// keep this, this informs XLS script data not exist
			} else {
				fprintf( stream, "%d\n", N_TAPS * 2 ) ;
				fprintf( stream, "            NLF                 LLF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				CommonPrintModule_FROC( stream, i, j, max_x_FROC ) ;
			}
		}
	}

	delete[] error_printed ;

	if( MAX_READERS > 1 ) {
		avg_parameters_arr = new float *[ N_TREATMENTS ] ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) avg_parameters_arr[ i ] = new float[ 5 ] ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			float *parms_temp0 = new float[ MAX_READERS ] ;
			float *parms_temp1 = new float[ MAX_READERS ] ;
			float *parms_temp2 = new float[ MAX_READERS ] ;
			float *parms_temp3 = new float[ MAX_READERS ] ;
			float *parms_temp4 = new float[ MAX_READERS ] ;
			int c1 = 0 ;
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( froc_fit_errors[ i ][ j ] == 0 ) {
					parms_temp0[ c1 ] = froc_fit_parms[ i ][ j ][ 0 ] ;
					parms_temp1[ c1 ] = froc_fit_parms[ i ][ j ][ 1 ] ;
					parms_temp2[ c1 ] = froc_fit_parms[ i ][ j ][ 2 ] ;
					parms_temp3[ c1 ] = froc_fit_parms[ i ][ j ][ 3 ] ;
					parms_temp4[ c1 ] = froc_fit_parms[ i ][ j ][ 4 ] ;
					c1++ ;
				}
			}
			avg_parameters_arr[ i ][ 0 ] = mean( parms_temp0, c1 ) ;	
			avg_parameters_arr[ i ][ 1 ] = mean( parms_temp1, c1 ) ;
			avg_parameters_arr[ i ][ 2 ] = mean( parms_temp2, c1 ) ;
			avg_parameters_arr[ i ][ 3 ] = mean( parms_temp3, c1 ) ;
			avg_parameters_arr[ i ][ 4 ] = mean( parms_temp4, c1 ) ;

			delete[] parms_temp0 ;
			delete[] parms_temp1 ;
			delete[] parms_temp2 ;
			delete[] parms_temp3 ;
			delete[] parms_temp4 ;
		}
		fprintf( stream, " Average reader curves for each modality\n" ) ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Modality:\n" ) ;
			fprintf( stream, "%s\n", modalityTable[ i ] ) ;
			if( avg_parameters_arr[ i ][ 0 ] == 0 ) {
				fprintf( stream, "0\n" ) ;
			} else {
				fprintf( stream, "%d\n", N_TAPS * 2 ) ;
				fprintf( stream, "            NLF                 LLF\n" ) ;
				fprintf( stream, "        -----------         -----------\n" ) ;
				CommonPrintModule_FROC( stream, i, -1, max_x_FROC ) ;
			}
		}

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) delete[] avg_parameters_arr[ i ] ;
		delete[] avg_parameters_arr ;
	}
	fclose( stream ) ;
}
