// DBM ANOVA DLL PROTOTYPES
extern "C" {
__declspec(dllimport) void ANOVA_DLL(
	int *rrrc, int *frrc, int *rrfc, int *ishowtxc, int *ishoworvarcomp,
	double *pv, double *tm, int *N_TREATMENTS, int *MAX_READERS, int *CASES, int *debug, int *ifile	) ;

__declspec(dllimport) void TREATMENT_READER_CASE_ANOVA(
	double *SS_T, double *SS_R, double *SS_C, double *SS_TR, double *SS_TC, double *SS_RC, double *SS_TRC, double *SS_TOTAL,
	double *MS_T, double *MS_R, double *MS_C, double *MS_TR, double *MS_TC, double *MS_RC, double *MS_TRC,
	double *DF_T, double *DF_R, double *DF_C, double *DF_TR, double *DF_TC, double *DF_RC, double *DF_TRC, double *DF_TOTAL ) ;

__declspec(dllimport) void READER_CASE_ANOVE_FOR_EACH_TREATMENT(
	double *MSr_T, double *MSc_T, double *MStrc_T ) ;

__declspec(dllimport) void VAR_COMP_ESTIMATES( 
	double *VAR_R, double *CI_VC_R_Low, double *CI_VC_R_High,
	double *VAR_C, double *CI_VC_C_Low, double *CI_VC_C_High,
	double *VAR_TR, double *CI_VC_TR_Low, double *CI_VC_TR_High,
	double *VAR_TC, double *CI_VC_TC_Low, double *CI_VC_TC_High,
	double *VAR_RC, double *CI_VC_RC_Low, double *CI_VC_RC_High,
	double *VAR_TRC, double *CI_VC_TRC_Low, double *CI_VC_TRC_High ) ;


__declspec(dllimport) void OBU_ROC_ESTIMATES( 
	double *COV1_JACK, double *COV2_JACK, double *COV3_JACK, double *VAR_JACK ) ;

__declspec(dllimport) void RANDOM_READERS_RANDOM_CASES( 
	double *F_T_RRRC, double *P_T_RRRC, double *DFErr_T_RRRC, double *MSErr_T_RRRC,
	double *TMeanDiffPSV, double *DSTDERR_RRRC, double *DNDIF_RRRC, double *T_TRT_Diff_Act_RDR_RRRC,                                
	double *T_TRT_Diff_P_RDR_RRRC, double *CI_T_Low_RRRC, double *CI_T_High_RRRC,                                
	double *Mn_T_PSV, double *STDERR_RRRC, double *DF_RxC_RRRC, double *CI_TRT_Low_RRRC, double *CI_TRT_High_RRRC ) ;


__declspec(dllimport) void FIXED_READERS_RANDOM_CASES( 
	double *F_T_FRRC, double *P_T_FRRC, double *DFErr_T_FRRC, double *MSErr_T_FRRC,                                
	double *TMeanDiffPSV, double *DSTDERR_FRRC, double *DNDIF_FRRC, double *T_TRT_Diff_Act_RDR_FRRC,                                
	double *T_TRT_Diff_P_RDR_FRRC, double *CI_T_Low_FRRC, double *CI_T_High_FRRC,                                
	double *Mn_T_PSV, double *STDERR_FRRC, double *CI_TRT_Low_FRRC, double *CI_TRT_High_FRRC ) ;

__declspec(dllimport) void FIXED_READERS_RANDOM_CASES2( 
	double *SSt_R, double *SSc_R, double *SStc_R,                               
	double *MSt_R, double *MSc_R, double *MStc_R,                                
	double *TMeanDiffRDR, double *SE_RDR_Tdiff, double *T_TRT_Diff_Act_TXC_FRRC,                                
	double *T_TRT_Diff_P_TXC_FRRC, double *CI_RDR_Low, double *CI_RDR_High ) ;

__declspec(dllimport) void RANDOM_READERS_FIXED_CASES( 
	double *F_T_RRFC, double *P_T_RRFC, double *DFErr_T_RRFC, double *MSErr_T_RRFC,                                
	double *TMeanDiffPSV, double *DSTDERR_RRFC, double *DNDIF_RRFC, double *T_TRT_Diff_Act_RDR_RRFC,                                
	double *T_TRT_Diff_P_RDR_RRFC, double *CI_T_Low_RRFC, double *CI_T_High_RRFC,                                
	double *Mn_T_PSV, double *STDERR_RRFC, double *CI_TRT_Low_RRFC, double *CI_TRT_High_RRFC ) ;
}

void alloc_anova_dll( void ) ;
void free_anova_dll( void ) ;
void retrieve_anova_dll( void ) ;

void RUN_ANOVA_DLL( int analylsis_method ) ;
void Print_ANOVA_Report( FILE *stream ) ;


// DATA OUTPUT
extern double SS_T, SS_R, SS_C, SS_TR, SS_TC, SS_RC, SS_TRC, SS_TOTAL ;
extern double MS_T, MS_R, MS_C, MS_TR, MS_TC, MS_RC, MS_TRC ;
extern double DF_T, DF_R, DF_C, DF_TR, DF_TC, DF_RC, DF_TRC, DF_TOTAL ;

extern double *MSr_T, *MSc_T, *MStrc_T ;	// (N_TREATMENTS)

extern double VAR_R, CI_VC_R_Low, CI_VC_R_High ;
extern double VAR_C, CI_VC_C_Low, CI_VC_C_High ;
extern double VAR_TR, CI_VC_TR_Low, CI_VC_TR_High ;
extern double VAR_TC, CI_VC_TC_Low, CI_VC_TC_High ;
extern double VAR_RC, CI_VC_RC_Low, CI_VC_RC_High ;
extern double VAR_TRC, CI_VC_TRC_Low, CI_VC_TRC_High ;

extern double COV1_JACK, COV2_JACK, COV3_JACK, VAR_JACK ;

extern double F_T_RRRC, P_T_RRRC, DFErr_T_RRRC, MSErr_T_RRRC ;

extern double *TMeanDiffPSV, *DSTDERR_RRRC, *DNDIF_RRRC, *T_TRT_Diff_Act_RDR_RRRC ;	// (N_TREATMENTS,N_TREATMENTS)
extern double *T_TRT_Diff_P_RDR_RRRC, *CI_T_Low_RRRC, *CI_T_High_RRRC ;				// (N_TREATMENTS,N_TREATMENTS)

extern double *Mn_T_PSV, *STDERR_RRRC, *DF_RxC_RRRC, *CI_TRT_Low_RRRC, *CI_TRT_High_RRRC ;	// (N_TREATMENTS)

extern double F_T_FRRC, P_T_FRRC, DFErr_T_FRRC, MSErr_T_FRRC ;                                

//double *TMeanDiffPSV,
extern double *DSTDERR_FRRC, *DNDIF_FRRC, *T_TRT_Diff_Act_RDR_FRRC ;				// (N_TREATMENTS,N_TREATMENTS)
extern double *T_TRT_Diff_P_RDR_FRRC, *CI_T_Low_FRRC, *CI_T_High_FRRC ;				// (N_TREATMENTS,N_TREATMENTS)
//double *Mn_T_PSV, 
extern double *STDERR_FRRC, *CI_TRT_Low_FRRC, *CI_TRT_High_FRRC ;					// (N_TREATMENTS)

extern double *SSt_R, *SSc_R, *SStc_R ;		// (MAX_READERS)
extern double *MSt_R, *MSc_R, *MStc_R ;		// (MAX_READERS)                          
extern double *TMeanDiffRDR, *SE_RDR_Tdiff, *T_TRT_Diff_Act_TXC_FRRC ;	// (MAX_READERS,N_TREATMENTS,N_TREATMENTS)                               
extern double *T_TRT_Diff_P_TXC_FRRC, *CI_RDR_Low, *CI_RDR_High ;		// (MAX_READERS,N_TREATMENTS,N_TREATMENTS)

extern double F_T_RRFC, P_T_RRFC, DFErr_T_RRFC, MSErr_T_RRFC ;

//double *TMeanDiffPSV, 
extern double *DSTDERR_RRFC, *DNDIF_RRFC, *T_TRT_Diff_Act_RDR_RRFC ;		// (N_TREATMENTS,N_TREATMENTS)
extern double *T_TRT_Diff_P_RDR_RRFC, *CI_T_Low_RRFC, *CI_T_High_RRFC ;		// (N_TREATMENTS,N_TREATMENTS)
//double *Mn_T_PSV, 

extern double *STDERR_RRFC, *CI_TRT_Low_RRFC, *CI_TRT_High_RRFC ;			// (N_TREATMENTS)


