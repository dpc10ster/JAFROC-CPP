#include "stdafx.h"
#include "../common.h"
#include <stdio.h>
#include <math.h>

#define RANDOM_ALL				200
#define RANDOM_CASES			201
#define RANDOM_READERS			202



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void Print_ANOVA_Results( FILE *report, int what_is_random )
{
	fprintf( report, "\n" ) ;
	fprintf( report, "\n" ) ;
	fprintf( report, " F-statistic and p-value\n" ) ;
	fprintf( report, "\n" ) ;
	fprintf( report, " Significance level of test (alpha) = 0.05\n" ) ;
	fprintf( report, " F-statistic  : %10.3f\n", Results[ 0 ] ) ;
	fprintf( report, " p-value      : %10.3E\n", Results[ 1 ] ) ;
	fprintf( report, " DF (Num,Den) : ( %2d, %10.3E )\n", (int)DF[ 0 ], DF[ 1 ] ) ;
	fprintf( report, "\n" ) ;
	fprintf( report, " N.B. If the p-value is less than 0.05 there is a significant difference\n" ) ;
	fprintf( report, "      between at least one pair of TREATMENTS\n" ) ;
	fprintf( report, "\n" ) ;

	float **READER_AVG_CI ;

	if( what_is_random == RANDOM_ALL ) READER_AVG_CI = TRT_CI_ALL ;
	if( what_is_random == RANDOM_CASES ) READER_AVG_CI = TRT_CI_CASES ;
	if( what_is_random == RANDOM_READERS ) READER_AVG_CI = TRT_CI_RDRS ;

	if( MAX_READERS > 1 ) {
		fprintf( report, " READER-averaged FOMs for TREATMENTS and 95%% confidence intervals:\n\n" ) ;
	} else {
		fprintf( report, " FOMs for TREATMENTS and 95%% confidence intervals:\n\n" ) ;
	}

	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( report, "%-10s %7.4f, (%7.4f,%7.4f)\n", modalityTable[ i ],
			mean( TR_MEANS[ i ], MAX_READERS ), READER_AVG_CI[ i ][ 0 ], READER_AVG_CI[ i ][ 1 ] ) ;
	}

	fprintf( report, "\n" ) ;
	fprintf( report, " INTER-TREATMENT differences and 95%% confidence intervals:\n" ) ;
	fprintf( report, "\n" ) ;

	int inc = 0 ;
	for( int i = 0 ; i < N_TREATMENTS - 1 ; i++ ) {
		for( int i1 = i + 1 ; i1 < N_TREATMENTS ; i1++ ) {
			if( strlen( modalityTable[i] ) > 10 )
				fprintf( report, " FOM(%10s)", modalityTable[i] ) ;
			else
				fprintf( report, " FOM(%s)", modalityTable[i] ) ;
			if( strlen( modalityTable[i1] ) > 10 )
				fprintf( report, " - FOM(%10s)", modalityTable[i1] ) ;
			else
				fprintf( report, " - FOM(%s)", modalityTable[i1] ) ;
			fprintf( report,
				" = %7.4f, (%7.4f,%7.4f )\n", 
				mean( TR_MEANS[ i ], MAX_READERS ) - mean( TR_MEANS[ i1 ], MAX_READERS ),
				Left[ inc ], Right[ inc ] ) ;
			inc++ ;
		}
	}

	fprintf( report, "\n" ) ;
	fprintf( report, "\n" ) ;
}


void Print_JK_Results_RANDOM_ALL( FILE *report )
{
	if( MAX_READERS == 1 ) {
		fprintf( report, "================================================================================\n" ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, " DATA SET HAS ONLY ONE READER\n" ) ;
		fprintf( report, " CANNOT DO RANDOM ALL ANALYSIS\n" ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, "================================================================================\n" ) ;
		return ;
	} else {
		int err ;
		anova( 
			N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], TR_MEANS, PseudoValues,
			ANOVA_RANDOM_ALL,
			MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
			Results, DF, Left, Right, TRT_CI_ALL, &err
			) ;

		if( err != 0 ) {
			WriteToConsole( "Error on ANOVA() random all" ) ;
			if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
				WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
				WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
				WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
				WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
				WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
				WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
				WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
				WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
				WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
			}
		}

	}
	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
			fprintf( report, " FOM = Trapezoidal area under AFROC' curve, including NLs on abnormal cases\n" ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC:
			fprintf( report, " FOM = Trapezoidal area under AFROC curve, ignoring NLs on abnormal cases\n" ) ;
			break ;
		case ANALYSIS_METHOD_HR_ROC:
			fprintf( report, " FOM = Trapezoidal area under ROC curve\n" ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
			fprintf( report, " FOM = Trapezoidal area under AFROC' curve, including NLs on abnormal cases\n" ) ;
			break ;
		case ANALYSIS_METHOD_JAFROC_WEIGHTED:
			fprintf( report, " FOM = Trapezoidal area under AFROC curve, ignoring NLs on abnormal cases\n" ) ;
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
			char s[ 11 ] ;
			strncpy( s, modalityTable[ mod_part_ind ], 10 ) ;
			s[ 10 ] = 0 ;
			fprintf( report, " %-10s", s ) ;
		}
		fprintf( report, "\n" ) ;
		fprintf( report, "%14s", " " ) ;
		for( int mod_part_ind = mod_rep_inc * 4 ; mod_part_ind < mod_rep_inc * 4 + mod_part_max ; mod_part_ind++ ) {
			fprintf( report, "%-10s ", "------" ) ;
		}
		fprintf( report, "\n" ) ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			fprintf( report, "%-14s", readerTable[ j ] ) ;
			for( int mod_part_ind = mod_rep_inc * 4 ; mod_part_ind < mod_rep_inc * 4 + mod_part_max ; mod_part_ind++ ) {
				fprintf( report, "%-10.4f ", TR_MEANS[ mod_part_ind ][ j ] ) ;
			}
			fprintf( report, "\n" ) ;
		}
	}
	fprintf( report, "\n" ) ;
	fprintf( report, "==================  Results: RANDOM_ALL  ==================\n" ) ;

	Print_ANOVA_Results( report, RANDOM_ALL ) ;
	
	fprintf( report, " Variance components needed for sample-size estimation:\n" ) ;
	fprintf( report, "   var_tr  : %12.4E\n", var_tr ) ;
	fprintf( report, "   var_tc  : %12.4E\n", var_tc ) ;
	fprintf( report, "   var_trc : %12.4E\n", var_trc ) ;
	fprintf( report, "\n" ) ;
}



void Print_JK_Results_RANDOM_CASES( FILE *report )
{
	int err ;
	anova(
		N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], TR_MEANS, PseudoValues,
		ANOVA_RANDOM_CASES,
		MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
		Results, DF, Left, Right, TRT_CI_CASES, &err
	) ;

	if( err != 0 ) {
		WriteToConsole( "Error on ANOVA() random cases" ) ;
		if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
			WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
			WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
			WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
			WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
			WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
			WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
			WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
			WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
		}
		if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
			WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
		}
	}

	fprintf( report, "\n" ) ;
	fprintf( report, "==================  Results: RANDOM_CASES  ==================\n" ) ;

	Print_ANOVA_Results( report, RANDOM_CASES ) ;
}



void Print_JK_Results_RANDOM_READERS( FILE *report )
{
	if( MAX_READERS == 1 ) {
		fprintf( report, "================================================================================\n" ) ;
		fprintf( report, "\n" ) ;
		fprintf( report, " DATA SET HAS ONLY ONE READER\n" ) ;
		fprintf( report, " CANNOT DO RANDOM READERS ANALYSIS\n" ) ;
		fprintf( report, "\n" ) ;
		return ;
	} else {
		int err ;
		anova(
			N_TREATMENTS, MAX_READERS, MAX_CASES[ 0 ], MAX_CASES[ 1 ], TR_MEANS, PseudoValues,
			ANOVA_RANDOM_READERS,
			MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC,
			Results, DF, Left, Right, TRT_CI_RDRS, &err
			) ;

		if( err != 0 ) {
			WriteToConsole( "Error on ANOVA() random readers" ) ;
			if( err & JAFROC_DLL_ERROR_ZERO_N_ABN ) {
				WriteToConsole( "Number of abnormal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_NOR ) {
				WriteToConsole( "Number of normal cases is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_NL ) {
				WriteToConsole( "Number of non-lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_N_MAX_LL ) {
				WriteToConsole( "Number of lesion localizations is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_READER ) {
				WriteToConsole( "Number of readers is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_ZERO_TREATMENT ) {
				WriteToConsole( "Number of treatments is set to zero.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_METHOD ) {
				WriteToConsole( "Invalid analysis method.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_FLOAT_ERROR ) {
				WriteToConsole( "Floating-point error occurred.\r\n\r\n" ) ;
			}
			if( err & JAFROC_DLL_ERROR_INVALID_ANOVA ) {
				WriteToConsole( "Invalid ANOVA analysis.\r\n\r\n" ) ;
			}
		}
	}

	fprintf( report, "\n" ) ;
	fprintf( report, "==================  Results: RANDOM_RDRS  ==================\n" ) ;

	Print_ANOVA_Results( report, RANDOM_READERS ) ;
}


void Print_JK_Results( FILE *report )
{
	Print_JK_Results_RANDOM_ALL( report ) ;
	Print_JK_Results_RANDOM_CASES( report ) ;
	Print_JK_Results_RANDOM_READERS( report ) ;

}