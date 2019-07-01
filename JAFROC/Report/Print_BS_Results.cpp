#include "stdafx.h"
#include "../common.h"
#include <stdio.h>
#include <math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void Print_BS_Results( FILE *report )
{
	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
			fprintf( report, " FOM = Trapezoidal area under AFROC' curve, including NLs on abnormal cases\n" ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC:
			fprintf( report, " FOM = Trapezoidal area under AFROC curve, ignoring NLs on abnormal cases\n" ) ;
			break ;
		case ANALYSIS_METHOD_ROC:
			fprintf( report, " FOM = Trapezoidal area under ROC curve\n" ) ;
			break ;
	}

	int len_rdr = 10 ;
	int len_trt = 10 ;
	int len3 = 10 - strlen( "0.0000" ) ;

	fprintf( report, "\n" ) ;
	fprintf( report, "\n" ) ;
	fprintf( report, " FOMs for TREATMENTS (columns) and READERS (rows)\n" ) ;
	fprintf( report, "\n" ) ;

	int mod_rep = (int)floor( ( (float)N_TREATMENTS - 0.1f ) / 4.0f ) ;
	for( int mod_rep_inc = 0 ; mod_rep_inc <= mod_rep ; mod_rep_inc++ ) {
		int mod_part_max ;
		if( mod_rep_inc == mod_rep ) {
			mod_part_max = N_TREATMENTS % 4 ;
			if( mod_part_max == 0 ) mod_part_max = 4 ;
		} else {
			mod_part_max = 4 ;
		}
		if( mod_rep_inc == 0 ) fprintf( report, "\n" ) ;
		fprintf( report, "%13s", " " ) ;
		for( int mod_part_ind = mod_rep_inc * 4 ; mod_part_ind < mod_rep_inc * 4 + mod_part_max ; mod_part_ind++ ) {
			fprintf( report, " %-10s", modalityTable[ mod_part_ind ].Left(10) ) ;
		}
		fprintf( report, "\n" ) ;
		fprintf( report, "%14s", " " ) ;
		for( int mod_part_ind = mod_rep_inc * 4 ; mod_part_ind < mod_rep_inc * 4 + mod_part_max ; mod_part_ind++ ) {
			fprintf( report, "%-10s ", "------" ) ;
		}
		fprintf( report, "\n" ) ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			fprintf( report, "%-14s", readerTable[ j ].Left(10) ) ;
			for( int mod_part_ind = mod_rep_inc * 4 ; mod_part_ind < mod_rep_inc * 4 + mod_part_max ; mod_part_ind++ ) {
				fprintf( report, "%-10.4f ", TR_MEANS[ mod_part_ind ][ j ] ) ;
			}
			fprintf( report, "\n" ) ;
		}
	}
	fprintf( report, "\n" ) ;
	fprintf( report, "==================  Results: NON-PARAMETRIC  ==================\n" ) ;

	fprintf( report, "\n" ) ;
	fprintf( report, " INTER-TREATMENT differences and 95%% confidence intervals:\n" ) ;
	fprintf( report, "\n" ) ;

	int inc = 0 ;
	for( int i = 0 ; i < N_TREATMENTS - 1 ; i++ ) {
		for( int i1 = i + 1 ; i1 < N_TREATMENTS ; i1++ ) {
			if( modalityTable[i].GetLength() > 10 )
				fprintf( report, " FOM(%10s)", modalityTable[i] ) ;
			else
				fprintf( report, " FOM(%s)", modalityTable[i] ) ;
			if( modalityTable[i1].GetLength() > 10 )
				fprintf( report, " - FOM(%10s)", modalityTable[i1] ) ;
			else
				fprintf( report, " - FOM(%s)", modalityTable[i1] ) ;
			fprintf( report,
				" = %7.4f, (%7.4f,%7.4f )\n", 
				mean( TR_MEANS[ i ], MAX_READERS ) - mean( TR_MEANS[ i1 ], MAX_READERS ),
				bs_lb[ inc ], bs_ub[ inc ] ) ;
			inc++ ;
		}
	}

	fprintf( report, "\n" ) ;
	fprintf( report, "\n" ) ;
	
}