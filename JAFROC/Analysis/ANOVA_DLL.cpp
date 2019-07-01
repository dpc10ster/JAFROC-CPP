#include "StdAfx.h"
#include "../common.h"

int rrrc = 1, frrc = 1, rrfc = 1, ishowtxc = 1, ishoworvarcomp = 1 ;

// DBM ANOVA DLL data array
// DATA OUTPUT
double SS_T, SS_R, SS_C, SS_TR, SS_TC, SS_RC, SS_TRC, SS_TOTAL ;
double MS_T, MS_R, MS_C, MS_TR, MS_TC, MS_RC, MS_TRC ;
double DF_T, DF_R, DF_C, DF_TR, DF_TC, DF_RC, DF_TRC, DF_TOTAL ;

double *MSr_T = NULL, *MSc_T, *MStrc_T ;	// (N_TREATMENTS)

double VAR_R, CI_VC_R_Low, CI_VC_R_High ;
double VAR_C, CI_VC_C_Low, CI_VC_C_High ;
double VAR_TR, CI_VC_TR_Low, CI_VC_TR_High ;
double VAR_TC, CI_VC_TC_Low, CI_VC_TC_High ;
double VAR_RC, CI_VC_RC_Low, CI_VC_RC_High ;
double VAR_TRC, CI_VC_TRC_Low, CI_VC_TRC_High ;

double COV1_JACK, COV2_JACK, COV3_JACK, VAR_JACK ;

double F_T_RRRC, P_T_RRRC, DFErr_T_RRRC, MSErr_T_RRRC ;

double *TMeanDiffPSV, *DSTDERR_RRRC, *DNDIF_RRRC, *T_TRT_Diff_Act_RDR_RRRC ;	// (N_TREATMENTS,N_TREATMENTS)
double *T_TRT_Diff_P_RDR_RRRC, *CI_T_Low_RRRC, *CI_T_High_RRRC ;				// (N_TREATMENTS,N_TREATMENTS)

double *Mn_T_PSV, *STDERR_RRRC, *DF_RxC_RRRC, *CI_TRT_Low_RRRC, *CI_TRT_High_RRRC ;	// (N_TREATMENTS)

double F_T_FRRC, P_T_FRRC, DFErr_T_FRRC, MSErr_T_FRRC ;                                

//double *TMeanDiffPSV,
double *DSTDERR_FRRC, *DNDIF_FRRC, *T_TRT_Diff_Act_RDR_FRRC ;				// (N_TREATMENTS,N_TREATMENTS)
double *T_TRT_Diff_P_RDR_FRRC, *CI_T_Low_FRRC, *CI_T_High_FRRC ;				// (N_TREATMENTS,N_TREATMENTS)
//double *Mn_T_PSV, 
double *STDERR_FRRC, *CI_TRT_Low_FRRC, *CI_TRT_High_FRRC ;					// (N_TREATMENTS)

double *SSt_R, *SSc_R, *SStc_R ;		// (MAX_READERS)
double *MSt_R, *MSc_R, *MStc_R ;		// (MAX_READERS)                          
double *TMeanDiffRDR, *SE_RDR_Tdiff, *T_TRT_Diff_Act_TXC_FRRC ;	// (MAX_READERS,N_TREATMENTS,N_TREATMENTS)                               
double *T_TRT_Diff_P_TXC_FRRC, *CI_RDR_Low, *CI_RDR_High ;		// (MAX_READERS,N_TREATMENTS,N_TREATMENTS)

double F_T_RRFC, P_T_RRFC, DFErr_T_RRFC, MSErr_T_RRFC ;

//double *TMeanDiffPSV, 
double *DSTDERR_RRFC, *DNDIF_RRFC, *T_TRT_Diff_Act_RDR_RRFC ;		// (N_TREATMENTS,N_TREATMENTS)
double *T_TRT_Diff_P_RDR_RRFC, *CI_T_Low_RRFC, *CI_T_High_RRFC ;		// (N_TREATMENTS,N_TREATMENTS)
//double *Mn_T_PSV, 

double *STDERR_RRFC, *CI_TRT_Low_RRFC, *CI_TRT_High_RRFC ;			// (N_TREATMENTS)




void alloc_anova_dll()
{
	if( MSr_T != NULL ) free_anova_dll() ;

	MSr_T = new double[ N_TREATMENTS ] ;
	MSc_T = new double[ N_TREATMENTS ] ;
	MStrc_T = new double[ N_TREATMENTS ] ;

	TMeanDiffPSV = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	DSTDERR_RRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	DNDIF_RRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_Act_RDR_RRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_P_RDR_RRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	CI_T_Low_RRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	CI_T_High_RRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;

	Mn_T_PSV = new double[ N_TREATMENTS ] ;
	STDERR_RRRC = new double[ N_TREATMENTS ] ;
	DF_RxC_RRRC = new double[ N_TREATMENTS ] ;
	CI_TRT_Low_RRRC = new double[ N_TREATMENTS ] ;
	CI_TRT_High_RRRC = new double[ N_TREATMENTS ] ;

	DSTDERR_FRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	DNDIF_FRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_Act_RDR_FRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_P_RDR_FRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	CI_T_Low_FRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	CI_T_High_FRRC = new double[ N_TREATMENTS * N_TREATMENTS ] ;

	STDERR_FRRC = new double[ N_TREATMENTS ] ;
	CI_TRT_Low_FRRC = new double[ N_TREATMENTS ] ;
	CI_TRT_High_FRRC = new double[ N_TREATMENTS ] ;

	SSt_R = new double[ MAX_READERS ] ;
	SSc_R = new double[ MAX_READERS ] ;
	SStc_R = new double[ MAX_READERS ] ;
	MSt_R = new double[ MAX_READERS ] ;
	MSc_R = new double[ MAX_READERS ] ;
	MStc_R = new double[ MAX_READERS ] ;

	TMeanDiffRDR = new double[ MAX_READERS * N_TREATMENTS * N_TREATMENTS ] ;
	SE_RDR_Tdiff = new double[ MAX_READERS * N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_Act_TXC_FRRC = new double[ MAX_READERS * N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_P_TXC_FRRC = new double[ MAX_READERS * N_TREATMENTS * N_TREATMENTS ] ;
	CI_RDR_Low = new double[ MAX_READERS * N_TREATMENTS * N_TREATMENTS ] ;
	CI_RDR_High = new double[ MAX_READERS * N_TREATMENTS * N_TREATMENTS ] ;

	DSTDERR_RRFC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	DNDIF_RRFC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_Act_RDR_RRFC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	T_TRT_Diff_P_RDR_RRFC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	CI_T_Low_RRFC = new double[ N_TREATMENTS * N_TREATMENTS ] ;
	CI_T_High_RRFC = new double[ N_TREATMENTS * N_TREATMENTS ] ;

	STDERR_RRFC = new double[ N_TREATMENTS ] ;
	CI_TRT_Low_RRFC = new double[ N_TREATMENTS ] ;
	CI_TRT_High_RRFC = new double[ N_TREATMENTS ] ;
}


void free_anova_dll( void )
{
	if( MSr_T == NULL ) return ;

	delete[] MSr_T ;
	delete[] MSc_T ;
	delete[] MStrc_T ;

	delete[] TMeanDiffPSV ;
	delete[] DSTDERR_RRRC ;
	delete[] DNDIF_RRRC ;
	delete[] T_TRT_Diff_Act_RDR_RRRC ;
	delete[] T_TRT_Diff_P_RDR_RRRC ;
	delete[] CI_T_Low_RRRC ;
	delete[] CI_T_High_RRRC ;

	delete[] Mn_T_PSV ;
	delete[] DF_RxC_RRRC ;
	delete[] STDERR_RRRC ;
	delete[] CI_TRT_Low_RRRC ;
	delete[] CI_TRT_High_RRRC ;

	delete[] DSTDERR_FRRC ;
	delete[] DNDIF_FRRC ;
	delete[] T_TRT_Diff_Act_RDR_FRRC ;
	delete[] T_TRT_Diff_P_RDR_FRRC ;
	delete[] CI_T_Low_FRRC ;
	delete[] CI_T_High_FRRC ;

	delete[] STDERR_FRRC ;
	delete[] CI_TRT_Low_FRRC ;
	delete[] CI_TRT_High_FRRC ;

	delete[] SSt_R ;
	delete[] SSc_R ;
	delete[] SStc_R ;
	delete[] MSt_R ;
	delete[] MSc_R ;
	delete[] MStc_R ;

	delete[] TMeanDiffRDR ;
	delete[] SE_RDR_Tdiff ;
	delete[] T_TRT_Diff_Act_TXC_FRRC ;
	delete[] T_TRT_Diff_P_TXC_FRRC ;
	delete[] CI_RDR_Low ;
	delete[] CI_RDR_High ;

	delete[] DSTDERR_RRFC ;
	delete[] DNDIF_RRFC ;
	delete[] T_TRT_Diff_Act_RDR_RRFC ;
	delete[] T_TRT_Diff_P_RDR_RRFC ;
	delete[] CI_T_Low_RRFC ;
	delete[] CI_T_High_RRFC ;

	delete[] STDERR_RRFC ;
	delete[] CI_TRT_Low_RRFC ;
	delete[] CI_TRT_High_RRFC ;

	MSr_T = NULL ;
}


void retrieve_anova_dll( void )
{
	alloc_anova_dll() ;

	TREATMENT_READER_CASE_ANOVA(
		&SS_T, &SS_R, &SS_C, &SS_TR, &SS_TC, &SS_RC, &SS_TRC, &SS_TOTAL,
		&MS_T, &MS_R, &MS_C, &MS_TR, &MS_TC, &MS_RC, &MS_TRC,
		&DF_T, &DF_R, &DF_C, &DF_TR, &DF_TC, &DF_RC, &DF_TRC, &DF_TOTAL ) ;

	READER_CASE_ANOVE_FOR_EACH_TREATMENT(
		MSr_T, MSc_T, MStrc_T ) ;

	VAR_COMP_ESTIMATES( 
		&VAR_R, &CI_VC_R_Low, &CI_VC_R_High,
		&VAR_C, &CI_VC_C_Low, &CI_VC_C_High,
		&VAR_TR, &CI_VC_TR_Low, &CI_VC_TR_High,
		&VAR_TC, &CI_VC_TC_Low, &CI_VC_TC_High,
		&VAR_RC, &CI_VC_RC_Low, &CI_VC_RC_High,
		&VAR_TRC, &CI_VC_TRC_Low, &CI_VC_TRC_High ) ;

	var_tr = VAR_TR ;
	var_tc = VAR_TC ;
	var_trc = VAR_TRC ;

	OBU_ROC_ESTIMATES( 
		&COV1_JACK, &COV2_JACK, &COV3_JACK, &VAR_JACK ) ;

	RANDOM_READERS_RANDOM_CASES( 
		&F_T_RRRC, &P_T_RRRC, &DFErr_T_RRRC, &MSErr_T_RRRC,
		TMeanDiffPSV, DSTDERR_RRRC, DNDIF_RRRC, T_TRT_Diff_Act_RDR_RRRC,                                
		T_TRT_Diff_P_RDR_RRRC, CI_T_Low_RRRC, CI_T_High_RRRC,                                
		Mn_T_PSV, STDERR_RRRC, DF_RxC_RRRC, CI_TRT_Low_RRRC, CI_TRT_High_RRRC ) ;

	FIXED_READERS_RANDOM_CASES( 
		&F_T_FRRC, &P_T_FRRC, &DFErr_T_FRRC, &MSErr_T_FRRC,                                
		TMeanDiffPSV, DSTDERR_FRRC, DNDIF_FRRC, T_TRT_Diff_Act_RDR_FRRC,                                
		T_TRT_Diff_P_RDR_FRRC, CI_T_Low_FRRC, CI_T_High_FRRC,                                
		Mn_T_PSV, STDERR_FRRC, CI_TRT_Low_FRRC, CI_TRT_High_FRRC ) ;

	FIXED_READERS_RANDOM_CASES2( 
		SSt_R, SSc_R, SStc_R,                               
		MSt_R, MSc_R, MStc_R,                                
		TMeanDiffRDR, SE_RDR_Tdiff, T_TRT_Diff_Act_TXC_FRRC,                                
		T_TRT_Diff_P_TXC_FRRC, CI_RDR_Low, CI_RDR_High ) ;

	RANDOM_READERS_FIXED_CASES( 
		&F_T_RRFC, &P_T_RRFC, &DFErr_T_RRFC, &MSErr_T_RRFC,                                
		TMeanDiffPSV, DSTDERR_RRFC, DNDIF_RRFC, T_TRT_Diff_Act_RDR_RRFC,                                
		T_TRT_Diff_P_RDR_RRFC, CI_T_Low_RRFC, CI_T_High_RRFC,                                
		Mn_T_PSV, STDERR_RRFC, CI_TRT_Low_RRFC, CI_TRT_High_RRFC ) ;
}


// DATA INPUT
//int rrrc = 1, frrc = 1, rrfc = 1, ishowtxc = 1, ishoworvarcomp = 1 ;
int CASES, debug, fileout ;
double *pseudo_values, *treatment_means ;

void RUN_ANOVA_DLL( int analysis_method )
{

	CASES = MAX_CASES[ 0 ] + MAX_CASES[ 1 ];

	if (analysis_method == ANALYSIS_METHOD_MAX_NLF) {
		CASES = MAX_CASES[ 0 ];
	} 
	if (analysis_method == ANALYSIS_METHOD_MAX_LLF) {
		CASES = MAX_CASES[ 1 ];
	} 
	if (analysis_method == ANALYSIS_METHOD_HR_SENSITIVITY) {
		CASES = MAX_CASES[ 1 ];
	} 
	if (analysis_method == ANALYSIS_METHOD_HR_SPECIFICITY) {
		CASES = MAX_CASES[ 0 ];
	} 
	if (analysis_method == ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY) {
		CASES = MAX_CASES[ 0 ];
	} 	
	if (analysis_method == ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS) {
	CASES = MAX_CASES[ 0 ] + MAX_CASES[ 1 ];
	} 
	debug = 0 ;
	fileout = 0 ;

	if( MAX_READERS > 1 ) {
		rrrc = 1 ;
		frrc = 1 ;
		rrfc = 1 ;
	} else {
		rrrc = 0 ;
		frrc = 1 ;
		rrfc = 0 ;
	}

	treatment_means = new double[ N_TREATMENTS * MAX_READERS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			treatment_means[ j * N_TREATMENTS + i ] = TR_MEANS[ i ][ j ] ;
		}
	}

	pseudo_values = new double[ N_TREATMENTS * MAX_READERS * CASES ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			for( int k = 0 ; k < CASES ; k++ ) {
				pseudo_values[ ( k * MAX_READERS + j ) * N_TREATMENTS+ i ] = PseudoValues[ i ][ j ][ k ] ;
			}
		}
	}

	ANOVA_DLL( &rrrc, &frrc, &rrfc, &ishowtxc, &ishoworvarcomp,
		pseudo_values, treatment_means, &N_TREATMENTS, &MAX_READERS, &CASES, &debug, &fileout ) ;

	retrieve_anova_dll() ;
	
	delete[] pseudo_values ;
	delete[] treatment_means ;
	
}


void print_overview( FILE *stream )
{
	fprintf( stream, " ====================================================================\n" ) ;
	fprintf( stream, " *****                        Overview                          *****\n" ) ;
	fprintf( stream, " ====================================================================\n" ) ;
	fprintf( stream, " Three analyses are presented: \n" ) ;
	fprintf( stream, " (1) Analysis 1 treats both readers and cases as random samples\n" ) ;
	fprintf( stream, "     --results apply to the reader and case populations;\n" ) ;
	fprintf( stream, " (2) Analysis 2 treats only cases as a random sample\n" ) ;
	fprintf( stream, "     --results apply to the population of cases but only for the\n" ) ;
	fprintf( stream, "     readers used in this study; and\n" ) ;
	fprintf( stream, " (3) Analysis 3 treats only readers as a random sample\n" ) ;
	fprintf( stream, "     --results apply to the population of readers but only for the\n" ) ;
	fprintf( stream, "     cases used in this study.\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " For all three analyses, the null hypothesis of equal treatments is\n" ) ;
	fprintf( stream, " tested in part (a), treatment difference 95%% confidence intervals\n" ) ;
	fprintf( stream, " are given in part (b), and treatment 95%% confidence intervals are\n" ) ;
	fprintf( stream, " given in part (c).  Parts (a) and (b) are based on the treatment x\n" ) ;
	fprintf( stream, " reader x case ANOVA while part (c) is based on the reader x case\n" ) ;
	fprintf( stream, " ANOVA for the specified treatment; these ANOVA tables are displayed\n" ) ;
	fprintf( stream, " before the analyses.  Different error terms are used as indicated\n" ) ;
	fprintf( stream, " for parts (a), (b), and (c) according to whether readers and cases\n" ) ;
	fprintf( stream, " are treated as fixed or random factors.  Note that the treatment\n" ) ;
	fprintf( stream, " confidence intervals in part (c) are based only on the data for the\n" ) ;
	fprintf( stream, " specified treatment, rather than the pooled data.  Treatment\n" ) ;
	fprintf( stream, " difference 95%% confidence intervals for each reader are presented\n" ) ;
	fprintf( stream, " in part (d) of Analysis 2; each interval is based on the treatment\n" ) ;
	fprintf( stream, " x case ANOVA table (not included) for the specified reader.\n" ) ;
}


void print_estimates( FILE *stream )
{
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " *****                            Estimates                            *****\n" ) ;
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "                        TREATMENT\n" ) ;
	fprintf( stream, "              " ) ; 
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "----------" ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "---" ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "  READER      " ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "%-10.10s", modalityTable[ i ] ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "----------    " ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "----------" ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
	}
	fprintf( stream, "\n" ) ;
	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		fprintf( stream, "%-10.10s    ", readerTable[ j ] ) ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "%10.8lf", TR_MEANS[ i ][ j ] ) ;
			if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
		}
		fprintf( stream, "\n" ) ;
	}
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " TREATMENT MEANS (averaged across readers)\n" ) ;
	fprintf( stream, "----------    -----------------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "%-10.10s    ", modalityTable[ i ] ) ;
		fprintf( stream, "%10.8lf\n", mean( TR_MEANS[ i ], MAX_READERS ) ) ;
	}
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, " TREATMENT MEAN DIFFERENCES\n" ) ;
	fprintf( stream, "----------   ----------    -----------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS - 1 ; i++ ) {
		for( int i1 = i + 1 ; i1 < N_TREATMENTS ; i1++ ) {
			fprintf( stream, "%-10.10s - %-10.10s    %10.8lf\n", 
				modalityTable[ i ], modalityTable[ i1 ], 
				mean( TR_MEANS[ i ], MAX_READERS ) - mean( TR_MEANS[ i1 ], MAX_READERS ) ) ;
		}
	}
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;

}


void print_anova_table( FILE *stream )
{
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " *****                          ANOVA Tables                           *****\n" ) ;
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " TREATMENT X READER X CASE ANOVA\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "Source            SS               DF             MS        \n" ) ;
	fprintf( stream, "------   --------------------    ------   ------------------\n" ) ;
	fprintf( stream, "     T   %20.8lf    %6d   %18.8lf\n", SS_T, (int)DF_T, MS_T ) ;
	fprintf( stream, "     R   %20.8lf    %6d   %18.8lf\n", SS_R, (int)DF_R, MS_R ) ;
	fprintf( stream, "     C   %20.8lf    %6d   %18.8lf\n", SS_C, (int)DF_C, MS_C ) ;
	fprintf( stream, "    TR   %20.8lf    %6d   %18.8lf\n", SS_TR, (int)DF_TR, MS_TR ) ;
	fprintf( stream, "    TC   %20.8lf    %6d   %18.8lf\n", SS_TC, (int)DF_TC, MS_TC ) ;
	fprintf( stream, "    RC   %20.8lf    %6d   %18.8lf\n", SS_RC, (int)DF_RC, MS_RC ) ;
	fprintf( stream, "   TRC   %20.8lf    %6d   %18.8lf\n", SS_TRC, (int)DF_TRC, MS_TRC ) ;
	fprintf( stream, " TOTAL   %20.8lf    %6d\n", SS_TOTAL, (int)DF_TOTAL ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " READER X CASE ANOVAs for each treatment\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "                        Mean Squares\n" ) ;
	fprintf( stream, " Source     df   " ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "%-10.10s", modalityTable[ i ] ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, " ------    ---   " ) ; for( int i = 0 ; i < N_TREATMENTS ; i++ ) fprintf( stream, "----------   " ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      R %6d   ", MAX_READERS - 1 ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "%10.8lf", MSr_T[ i ] ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "      C %6d   ", MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "%10.8lf", MSc_T[ i ] ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "     RC %6d   ", ( MAX_READERS - 1 ) * ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ) ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "%10.8lf", MStrc_T[ i ] ) ;
		if( i < N_TREATMENTS - 1 ) fprintf( stream, "   " ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
}


void print_var_comp( FILE *stream )
{
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " *****                  Variance components estimates                  *****\n" ) ;
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " DBM Variance Component Estimates\n" ) ;
	fprintf( stream, " (for sample size estimation for future studies)\n" ) ;
	fprintf( stream, " Note: These are unbiased ANOVA estimates which can be negative\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "     DBM Component            Estimate    \n" ) ;
	fprintf( stream, " -----------------------  ----------------\n" ) ;
	fprintf( stream, " Var(R)                   %16.8lf\n", VAR_R ) ;
	fprintf( stream, " Var(C)                   %16.8lf\n", VAR_C ) ;
	fprintf( stream, " Var(T*R)                 %16.8lf\n", VAR_TR ) ;
	fprintf( stream, " Var(T*C)                 %16.8lf\n", VAR_TC ) ;
	fprintf( stream, " Var(R*C)                 %16.8lf\n", VAR_RC ) ;
	fprintf( stream, " Var(T*R*C) + Var(Error)  %16.8lf\n", VAR_TRC ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " Obuchowski-Rockette variance component and covariance estimates\n" ) ;
	fprintf( stream, " (for sample size estimation for future studies)\n" ) ;
	fprintf( stream, " Note: These are unbiased ANOVA estimates which can be negative\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "     OR Component             Estimate    \n" ) ;
	fprintf( stream, " -----------------------  ----------------\n" ) ;
	fprintf( stream, " Reader                   %16.8lf\n", VAR_R ) ;
	fprintf( stream, " Treatment*Reader         %16.8lf\n", VAR_TR ) ;
	fprintf( stream, " COV1                     %16.8lf\n", COV1_JACK ) ;
	fprintf( stream, " COV2                     %16.8lf\n", COV2_JACK ) ;
	fprintf( stream, " COV3                     %16.8lf\n", COV3_JACK ) ;
	fprintf( stream, " ERROR                    %16.8lf\n", VAR_JACK ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
}


void print_rrrc( FILE *stream )
{
	char *fom_str ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC1 ) fom_str = "ANALYSIS_METHOD_JAFROC1" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC ) fom_str = "ANALYSIS_METHOD_JAFROC" ;
	if( analysis_method == ANALYSIS_METHOD_HR_ROC ) fom_str = "ANALYSIS_METHOD_HR_ROC" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC1_WEIGHTED ) fom_str = "ANALYSIS_METHOD_JAFROC1_WEIGHTED" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC_WEIGHTED ) fom_str = "ANALYSIS_METHOD_JAFROC_WEIGHTED";
	if( analysis_method == ANALYSIS_METHOD_SONG_FOM_A1 ) fom_str = "ANALYSIS_METHOD_SONG_FOM_A1" ;
	if( analysis_method == ANALYSIS_METHOD_SONG_FOM_A2 ) fom_str = "ANALYSIS_METHOD_SONG_FOM_A1";
	if( analysis_method == ANALYSIS_METHOD_ROI ) fom_str = "ANALYSIS_METHOD_ROI" ;
	if( analysis_method == ANALYSIS_METHOD_HR_SENSITIVITY ) fom_str = "ANALYSIS_METHOD_HR_SENSITIVITY" ;
	if( analysis_method == ANALYSIS_METHOD_HR_SPECIFICITY ) fom_str = "ANALYSIS_METHOD_HR_SPECIFICITY" ;
	if( analysis_method == ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY ) fom_str = "ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY" ;
	if( analysis_method == ANALYSIS_METHOD_MAX_NLF ) fom_str = "ANALYSIS_METHOD_MAX_NLF" ;
	if( analysis_method == ANALYSIS_METHOD_MAX_LLF ) fom_str = "ANALYSIS_METHOD_MAX_LLF" ;

	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " *****           Analysis 1: Random Readers and Random Cases           *****\n" ) ;
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " (Results apply to the population of readers and cases)\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    a) Test for H0: Treatments have the same %s\n", fom_str ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " Source        DF    Mean Square      F value  Pr > F \n" ) ;
	fprintf( stream, " ----------  ------  ---------------  -------  -------\n" ) ;
	if( P_T_RRRC >= SMALLEST_DISPLAYED_PVAL )
		fprintf( stream, " Treatment   %6d  %15.8lf  %7.2lf  %7.4lf\n", (int)DF_T, MS_T, F_T_RRRC, P_T_RRRC ) ;
	else 
		fprintf( stream, " Treatment   %6d  %15.8lf  %7.2lf  <%6.4lf\n", (int)DF_T, MS_T, F_T_RRRC, SMALLEST_DISPLAYED_PVAL ) ;
	fprintf( stream, " Error       %6.2lf  %15.8lf\n", DFErr_T_RRRC, MSErr_T_RRRC ) ;
	fprintf( stream, " Error term: MS(TR) + max[MS(TC)-MS(TRC),0]\n" ) ;
	fprintf( stream, " \n" ) ;
	if( P_T_RRRC < ALPHA ) {
		fprintf( stream, " Conclusion: The treatment %ss are not equal,\n             F(%d,%d) = %3.2lf, p = %6.4lf.\n", fom_str, (int)DF_T, (int)(DFErr_T_RRRC+0.5), F_T_RRRC, P_T_RRRC ) ;
	} else {
		fprintf( stream, " Conclusion: The treatment %ss are not significantly different,\n             F(%d,%d) = %3.2lf, p = %6.4lf.\n", fom_str, (int)DF_T, (int)(DFErr_T_RRRC+0.5), F_T_RRRC, P_T_RRRC ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    b) 95%% confidence intervals for treatment differences\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "       Treatment         Estimate   StdErr      DF      t     Pr > t          95%% CI      \n" ) ;
	fprintf( stream, "----------   ----------  --------  --------  -------  ------  -------  -------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS - 1 ; i++ ) {
		for( int j = i + 1 ; j < N_TREATMENTS ; j++ ) {
			fprintf( stream, "%-10.10s - %-10.10s  %8.5lf  %8.5lf  %7.2lf  %6.2lf  %7.4lf  %8.5lf , %8.5lf\n", modalityTable[ i ], modalityTable[ j ], 
				TMeanDiffPSV[j*N_TREATMENTS+i],DSTDERR_RRRC[j*N_TREATMENTS+i],DNDIF_RRRC[j*N_TREATMENTS+i],T_TRT_Diff_Act_RDR_RRRC[j*N_TREATMENTS+i],
				T_TRT_Diff_P_RDR_RRRC[j*N_TREATMENTS+i],CI_T_Low_RRRC[j*N_TREATMENTS+i],CI_T_High_RRRC[j*N_TREATMENTS+i] ) ;
		}
	}
	fprintf( stream, " \n" ) ;
	if( N_TREATMENTS == 2 ) {
		fprintf( stream, " H0: the two treatments are equal.\n" ) ;
	} else {
		fprintf( stream, " * H0: the %d treatments are equal.  To control the overall \n", N_TREATMENTS ) ;
		fprintf( stream, " type I error rate at .05, we conclude that treatment differences\n" ) ;
		fprintf( stream, " with p < .05 are significant only if the global test in \n" ) ;
		fprintf( stream, " (a) is also significant (i.e, p < .05).\n" ) ;
	}
	fprintf( stream, " Error term: MS(TR) + max[MS(TC)-MS(TRC),0]\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    c) 95%% treatment confidence intervals based on reader x case ANOVAs\n" ) ;
	fprintf( stream, "       for each treatment (each analysis is based only on data for the\n" ) ;
	fprintf( stream, "       specified treatment\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "  Treatment     Area      Std Error     DF     95%% Confidence Interval \n" ) ;
	fprintf( stream, "  ----------  ----------  ----------  -------  -------------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "  %-10.10s  %10.8lf  %10.8lf  %7.2lf  (%10.8lf , %10.8lf)\n", 
			modalityTable[ i ], Mn_T_PSV[ i ], STDERR_RRRC[ i ], DF_RxC_RRRC[ i ], CI_TRT_Low_RRRC[ i ], CI_TRT_High_RRRC[ i ] ) ;
	}
	fprintf( stream, " Error term: MS(R) + max[MS(C)-MS(RC),0]\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
}


void print_frrc( FILE *stream )
{
	char *fom_str ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC1 ) fom_str = "ANALYSIS_METHOD_JAFROC1" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC ) fom_str = "ANALYSIS_METHOD_JAFROC" ;
	if( analysis_method == ANALYSIS_METHOD_HR_ROC ) fom_str = "ANALYSIS_METHOD_HR_ROC" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC1_WEIGHTED ) fom_str = "ANALYSIS_METHOD_JAFROC1_WEIGHTED" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC_WEIGHTED ) fom_str = "ANALYSIS_METHOD_JAFROC_WEIGHTED";
	if( analysis_method == ANALYSIS_METHOD_SONG_FOM_A1 ) fom_str = "ANALYSIS_METHOD_SONG_FOM_A1" ;
	if( analysis_method == ANALYSIS_METHOD_SONG_FOM_A2 ) fom_str = "ANALYSIS_METHOD_SONG_FOM_A1";
	if( analysis_method == ANALYSIS_METHOD_ROI ) fom_str = "ANALYSIS_METHOD_ROI" ;
	if( analysis_method == ANALYSIS_METHOD_HR_SENSITIVITY ) fom_str = "ANALYSIS_METHOD_HR_SENSITIVITY" ;
	if( analysis_method == ANALYSIS_METHOD_HR_SPECIFICITY ) fom_str = "ANALYSIS_METHOD_HR_SPECIFICITY" ;
	if( analysis_method == ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY ) fom_str = "ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY" ;
	if( analysis_method == ANALYSIS_METHOD_MAX_NLF ) fom_str = "ANALYSIS_METHOD_MAX_NLF" ;
	if( analysis_method == ANALYSIS_METHOD_MAX_LLF ) fom_str = "ANALYSIS_METHOD_MAX_LLF" ;

	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " *****           Analysis 2: Fixed Readers and Random Cases            *****\n" ) ;
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " (Results apply to the population of cases but only for the readers\n" ) ;
	fprintf( stream, " used in this study)\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "    a) Test for H0: Treatments have the same %s\n", fom_str ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " Source        DF    Mean Square      F value  Pr > F \n" ) ;
	fprintf( stream, " ----------  ------  ---------------  -------  -------\n" ) ;
	if( P_T_FRRC >= SMALLEST_DISPLAYED_PVAL )
		fprintf( stream, " Treatment   %6d  %15.8lf  %7.2lf  %7.4lf\n", (int)DF_T, MS_T, F_T_FRRC, P_T_FRRC ) ;
	else 
		fprintf( stream, " Treatment   %6d  %15.8lf  %7.2lf  <%6.4lf\n", (int)DF_T, MS_T, F_T_FRRC, SMALLEST_DISPLAYED_PVAL ) ;
	fprintf( stream, " Error       %6d  %15.8lf\n", (int)DFErr_T_FRRC, MSErr_T_FRRC ) ;
	fprintf( stream, " Error term: MS(TC)\n" ) ;
	fprintf( stream, " \n" ) ;
	if( P_T_FRRC < ALPHA ) {
		fprintf( stream, " Conclusion: The treatment %ss are not equal,\n             F(%d,%d) = %3.2lf, p = %6.4lf.\n", fom_str, (int)DF_T, (int)(DFErr_T_FRRC+0.5), F_T_FRRC, P_T_FRRC ) ;
	} else {
		fprintf( stream, " Conclusion: The treatment %ss are not significantly different,\n             F(%d,%d) = %3.2lf, p = %6.4lf.\n", fom_str, (int)DF_T, (int)(DFErr_T_FRRC+0.5), F_T_FRRC, P_T_FRRC ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    b) 95%% confidence intervals for treatment differences\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "       Treatment         Estimate   StdErr      DF      t     Pr > t          95%% CI      \n" ) ;
	fprintf( stream, "----------   ----------  --------  --------  -------  ------  -------  -------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS - 1 ; i++ ) {
		for( int j = i + 1 ; j < N_TREATMENTS ; j++ ) {
			fprintf( stream, "%-10.10s - %-10.10s  %8.5lf  %8.5lf  %7d  %6.2lf  %7.4lf  %8.5lf , %8.5lf\n", modalityTable[ i ], modalityTable[ j ], 
				TMeanDiffPSV[j*N_TREATMENTS+i],DSTDERR_FRRC[j*N_TREATMENTS+i],(int)DNDIF_FRRC[j*N_TREATMENTS+i],T_TRT_Diff_Act_RDR_FRRC[j*N_TREATMENTS+i],
				T_TRT_Diff_P_RDR_FRRC[j*N_TREATMENTS+i],CI_T_Low_FRRC[j*N_TREATMENTS+i],CI_T_High_FRRC[j*N_TREATMENTS+i] ) ;
		}
	}
	fprintf( stream, " \n" ) ;
	if( N_TREATMENTS == 2 ) {
		fprintf( stream, " H0: the two treatments are equal.\n" ) ;
	} else {
		fprintf( stream, " * H0: the %d treatments are equal.  To control the overall \n", N_TREATMENTS ) ;
		fprintf( stream, " type I error rate at .05, we conclude that treatment differences\n" ) ;
		fprintf( stream, " with p < .05 are significant only if the global test in \n" ) ;
		fprintf( stream, " (a) is also significant (i.e, p < .05).\n" ) ;
	}
	fprintf( stream, " Error term: MS(TC)\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    c) 95%% treatment confidence intervals based on reader x case ANOVAs\n" ) ;
	fprintf( stream, "       for each treatment (each analysis is based only on data for the\n" ) ;
	fprintf( stream, "       specified treatment\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "  Treatment     Area      Std Error     DF     95%% Confidence Interval \n" ) ;
	fprintf( stream, "  ----------  ----------  ----------  -------  -------------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "  %-10.10s  %10.8lf  %10.8lf  %7d  (%10.8lf , %10.8lf)\n", 
			modalityTable[ i ], Mn_T_PSV[ i ], STDERR_FRRC[ i ], (int)(DF_C), CI_TRT_Low_FRRC[ i ], CI_TRT_High_FRRC[ i ] ) ;
	}
	fprintf( stream, " Error term: MS(C)\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " TREATMENT X CASE ANOVAs for each reader\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "                        Sum of Squares\n" ) ;
	fprintf( stream, " Source     df   " ) ; for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%-11.11s   ", readerTable[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, " ------    ---   " ) ; for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "-----------   ", j + 1 ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      T %6d   ", N_TREATMENTS - 1 ) ;  for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%11.7lf   ", SSt_R[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      C %6d   ", ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ) ) ;  for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%11.7lf   ", SSc_R[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "     TC %6d   ", (N_TREATMENTS-1)*(MAX_CASES[0]+MAX_CASES[1]-1) ) ;  for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%11.7lf   ", SStc_R[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "                        Mean Squares\n" ) ;
	fprintf( stream, " Source     df   " ) ; for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%-11.11s   ", readerTable[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, " ------    ---   " ) ; for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "-----------   ", j + 1 ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      T %6d   ", N_TREATMENTS - 1 ) ;  for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%11.7lf   ", MSt_R[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      C %6d   ", ( MAX_CASES[ 0 ] + MAX_CASES[ 1 ] - 1 ) ) ;  for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%11.7lf   ", MSc_R[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "     TC %6d   ", (N_TREATMENTS-1)*(MAX_CASES[0]+MAX_CASES[1]-1) ) ;  for( int j = 0 ; j < MAX_READERS ; j++ ) fprintf( stream, "%11.7lf   ", MStc_R[ j ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    d) Treatment-by-case ANOVA CIs for each reader \n" ) ;
	fprintf( stream, "       (each analysis is based only on data for the specified reader)\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "  Reader         Treatment        Estimate  StdErr       DF      t     Pr > t          95%% CI      \n" ) ;
	fprintf( stream, "---------- ---------- ----------  --------  --------  -------  ------  -------  -------------------\n" ) ;
	for( int i = 0 ; i < MAX_READERS ; i++ ) {
		for( int j = 0 ; j < N_TREATMENTS - 1 ; j++ ) {
			for( int k = j + 1 ; k < N_TREATMENTS ; k++ ) {
				fprintf( stream, "%-10.10s %-10.10s-%-10.10s  %8.5lf  %8.5lf  %7d  %6.2f  %7.4lf  %8.5lf , %8.5lf\n",
					readerTable[ i ], modalityTable[ j ], modalityTable[ k ],
					TMeanDiffRDR[(k*N_TREATMENTS+j)*MAX_READERS+i],SE_RDR_Tdiff[(k*N_TREATMENTS+j)*MAX_READERS+i],(int)DF_C,T_TRT_Diff_Act_TXC_FRRC[(k*N_TREATMENTS+j)*MAX_READERS+i],
					T_TRT_Diff_P_TXC_FRRC[(k*N_TREATMENTS+j)*MAX_READERS+i],CI_RDR_Low[(k*N_TREATMENTS+j)*MAX_READERS+i],CI_RDR_High[(k*N_TREATMENTS+j)*MAX_READERS+i] ) ;
			}
		}
	}
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
}


void print_rrfc( FILE *stream )
{
	char *fom_str ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC1 ) fom_str = "ANALYSIS_METHOD_JAFROC1" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC ) fom_str = "ANALYSIS_METHOD_JAFROC" ;
	if( analysis_method == ANALYSIS_METHOD_HR_ROC ) fom_str = "ANALYSIS_METHOD_HR_ROC" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC1_WEIGHTED ) fom_str = "ANALYSIS_METHOD_JAFROC1_WEIGHTED" ;
	if( analysis_method == ANALYSIS_METHOD_JAFROC_WEIGHTED ) fom_str = "ANALYSIS_METHOD_JAFROC_WEIGHTED";
	if( analysis_method == ANALYSIS_METHOD_SONG_FOM_A1 ) fom_str = "ANALYSIS_METHOD_SONG_FOM_A1" ;
	if( analysis_method == ANALYSIS_METHOD_SONG_FOM_A2 ) fom_str = "ANALYSIS_METHOD_SONG_FOM_A1";
	if( analysis_method == ANALYSIS_METHOD_ROI ) fom_str = "ANALYSIS_METHOD_ROI" ;
	if( analysis_method == ANALYSIS_METHOD_HR_SENSITIVITY ) fom_str = "ANALYSIS_METHOD_HR_SENSITIVITY" ;
	if( analysis_method == ANALYSIS_METHOD_HR_SPECIFICITY ) fom_str = "ANALYSIS_METHOD_HR_SPECIFICITY" ;
	if( analysis_method == ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY ) fom_str = "ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY" ;
	if( analysis_method == ANALYSIS_METHOD_MAX_NLF ) fom_str = "ANALYSIS_METHOD_MAX_NLF" ;
	if( analysis_method == ANALYSIS_METHOD_MAX_LLF ) fom_str = "ANALYSIS_METHOD_MAX_LLF" ;

	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " *****           Analysis 3: Random Readers and Fixed Cases            *****\n" ) ;
	fprintf( stream, " ===========================================================================\n" ) ;
	fprintf( stream, " (Results apply to the population of readers but only for the cases used in this study)\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    a) Test for H0: Treatments have the same %s\n", fom_str ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, " Source        DF    Mean Square      F value  Pr > F \n" ) ;
	fprintf( stream, " ----------  ------  ---------------  -------  -------\n" ) ;
	if( P_T_RRFC >= SMALLEST_DISPLAYED_PVAL )
		fprintf( stream, " Treatment   %6d  %15.8lf  %7.2lf  %7.4lf\n", (int)DF_T, MS_T, F_T_RRFC, P_T_RRFC ) ;
	else 
		fprintf( stream, " Treatment   %6d  %15.8lf  %7.2lf  <%6.4lf\n", (int)DF_T, MS_T, F_T_RRFC, SMALLEST_DISPLAYED_PVAL ) ;
	fprintf( stream, " Error       %6.2lf  %15.8lf\n", DFErr_T_RRFC, MSErr_T_RRFC ) ;
	fprintf( stream, " Error term: MS(TR)\n" ) ;
	fprintf( stream, " \n" ) ;
	if( P_T_RRFC < ALPHA ) {
		fprintf( stream, " Conclusion: The treatment %ss are not equal,\n             F(%d,%d) = %3.2lf, p = %6.4lf.\n", fom_str, (int)DF_T, (int)(DFErr_T_RRFC+0.5), F_T_RRFC, P_T_RRFC ) ;
	} else {
		fprintf( stream, " Conclusion: The treatment %ss are not significantly different,\n             F(%d,%d) = %3.2lf, p = %6.4lf.\n", fom_str, (int)DF_T, (int)(DFErr_T_RRFC+0.5), F_T_RRFC, P_T_RRFC ) ;
	}
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    b) 95%% confidence intervals for treatment differences\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "       Treatment         Estimate   StdErr      DF      t     Pr > t          95%% CI      \n" ) ;
	fprintf( stream, "----------   ----------  --------  --------  -------  ------  -------  -------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS - 1 ; i++ ) {
		for( int j = i + 1 ; j < N_TREATMENTS ; j++ ) {
			fprintf( stream, "%-10.10s - %-10.10s  %8.5lf  %8.5lf  %7.2lf  %6.2lf  %7.4lf  %8.5lf , %8.5lf\n", modalityTable[ i ], modalityTable[ j ], 
				TMeanDiffPSV[j*N_TREATMENTS+i],DSTDERR_RRFC[j*N_TREATMENTS+i],DNDIF_RRFC[j*N_TREATMENTS+i],T_TRT_Diff_Act_RDR_RRFC[j*N_TREATMENTS+i],
				T_TRT_Diff_P_RDR_RRFC[j*N_TREATMENTS+i],CI_T_Low_RRFC[j*N_TREATMENTS+i],CI_T_High_RRFC[j*N_TREATMENTS+i] ) ;
		}
	}
	fprintf( stream, " \n" ) ;
	if( N_TREATMENTS == 2 ) {
		fprintf( stream, " H0: the two treatments are equal.\n" ) ;
	} else {
		fprintf( stream, " * H0: the %d treatments are equal.  To control the overall \n", N_TREATMENTS ) ;
		fprintf( stream, " type I error rate at .05, we conclude that treatment differences\n" ) ;
		fprintf( stream, " with p < .05 are significant only if the global test in \n" ) ;
		fprintf( stream, " (a) is also significant (i.e, p < .05).\n" ) ;
	}
	fprintf( stream, " \n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "    c) Reader-by-case ANOVAs for each treatment (each analysis is based\n" ) ;
	fprintf( stream, "       only on data for the specified treatment)\n" ) ;
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "                        Mean Squares\n" ) ;
	fprintf( stream, " Source     df   " ) ; for( int i = 0 ; i < N_TREATMENTS ; i++ ) fprintf( stream, "%-10.10s   ", modalityTable[ i ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, " ------    ---   " ) ; for( int i = 0 ; i < N_TREATMENTS ; i++ ) fprintf( stream, "----------   " ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      R %6d   ",MAX_READERS-1 ) ; for( int i = 0 ; i < N_TREATMENTS ; i++ ) fprintf( stream, "%10.8lf   ", MSr_T[ i ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "      C %6d   ",MAX_CASES[0]+MAX_CASES[1]-1 ) ; for( int i = 0 ; i < N_TREATMENTS ; i++ ) fprintf( stream, "%10.8lf   ", MSc_T[ i ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "     RC %6d   ",(MAX_READERS-1)*(MAX_CASES[0]+MAX_CASES[1]-1) ) ; for( int i = 0 ; i < N_TREATMENTS ; i++ ) fprintf( stream, "%10.8lf   ", MStrc_T[ i ] ) ; fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, " Estimates and 95%% Confidence Intervals\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "  Treatment     Area      Std Error     DF     95%% Confidence Interval \n" ) ;
	fprintf( stream, "  ----------  ----------  ----------  -------  -------------------------\n" ) ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "  %-10.10s  %10.8lf  %10.8lf  %7d  (%10.8lf , %10.8lf)\n", 
			modalityTable[ i ], Mn_T_PSV[ i ], STDERR_RRFC[ i ], (int)DF_R, CI_TRT_Low_RRFC[ i ], CI_TRT_High_RRFC[ i ] ) ;
	}
	fprintf( stream, " \n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
	fprintf( stream, "\n" ) ;
}


void print_references( FILE *stream )
{
fprintf( stream, "                               REFERENCES\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Dorfman, D.D., Berbaum, K.S., & Metz, C.E. (1992). Receiver operating\n" ) ;
fprintf( stream, " characteristic rating analysis: Generalization to the population of \n" ) ;
fprintf( stream, " readers and patients with the jackknife method. Investigative Radiology,\n" ) ;
fprintf( stream, " 27, 723-731.\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Dorfman, D.D., Berbaum, K.S., Lenth, R.V., Chen, Y.F., & Donaghy, B.A. (1998). \n" ) ;
fprintf( stream, " Monte Carlo validation of a multireader method for receiver operating characteristic \n" ) ;
fprintf( stream, " discrete rating data: Factorial experimental design. \n" ) ;
fprintf( stream, " Academic Radiology, 5, 591-602.\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Hillis, S.L., & Berbaum, K.S. (2004). Power estimation for the\n" ) ;
fprintf( stream, " Dorfman-Berbaum-Metz method. Academic Radiology, 11, 1260-1273.\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Hillis, S.L., Obuchowski, N.A., Schartz, K.M., & Berbaum, K.S.\n" ) ;
fprintf( stream, " (2005). A comparison of the Dorfman-Berbaum-Metz and Obuchowski-Rockette\n" ) ;
fprintf( stream, " methods for receiver operating characteristic (ROC) data. \n" ) ;
fprintf( stream, " Statistics in Medicine, 24, 1579-1607  DOI:10.1002/sim.2024.\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Hillis, S.L. (2005). Monte Carlo validation of the Dorfman-Berbaum-Metz\n" ) ;
fprintf( stream, " method using normalized pseudovalues and less data-based model simplification\n" ) ;
fprintf( stream, " Academic Radiology, 12:1534-1541  DOI:10.1016/j.acra.2005.07.012.\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Hillis, S.L. (2007). A comparison of denominator degrees of freedom for\n" ) ;
fprintf( stream, " multiple observer ROC analysis.  Statistics in Medicine, 26:596-619  DOI:10.1002/sim.2532.\n" ) ;
fprintf( stream, "\n" ) ;
fprintf( stream, "      Hillis, S.L., Berbaum, K.S., & Metz, C.E. (2008). Recent developments in the\n" ) ;
fprintf( stream, " Dorfman-Berbaum-Metz procedure for multireader ROC study analysis. Academic Radiology, in press.\n" ) ;
fprintf( stream, "\n" ) ;
}


void Print_ANOVA_Report( FILE *stream )
{
	print_overview( stream ) ;
	print_estimates( stream ) ;
	if( rrrc ) print_anova_table( stream ) ;
	if( rrrc ) print_var_comp( stream ) ;

	if( rrrc ) print_rrrc( stream ) ;
	if( frrc ) print_frrc( stream ) ;
	if( rrfc ) print_rrfc( stream ) ;

	print_references( stream ) ;
}