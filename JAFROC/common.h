// set development state
#include "resource.h"
#define JAFROC_DEVEL	1


// predefined analysis condition
#define MINIMUM_NUMBER_OF_CASES	15
#define MIN_NUMBER_OF_BINS		2
#define MIN_BIN_COUNT			5
#define	MAX_CTFFS				15

#define	UNINITIALIZED	-2000.0f

#define SMALLEST_DISPLAYED_PVAL		0.0001

// Common values
#define ALPHA			0.05f
#define MY_EPS			1e-12f
#define MAX_F			1e3f
#define MAX_DDF			1e3f
#define MIN_DDF			5e-1f

#define	HALF			0.5f
#define HALF_ROOT_TWO	0.5f * sqrt( 2.0f )
#define LAMBDA_MIN		0.01f
#define	LAMBDA_MAX		20.0f
#define BETA_MIN		0.01f
#define BETA_MAX		20.0f
#define MU_MIN			0.01f
#define MU_MAX			10.0f

#define TOL 1.0e-2
#define EQL 1.0e-4

#define MAX_PARAMETERS	30
#define MAX_BS_COUNT	2000


// program flow definitions
#define ANALYSIS_METHOD_HR_ROC			IDM_ROC_HR_AUC
#define ANALYSIS_METHOD_HR_SENSITIVITY		IDM_ROC_HR_SENSITIVITY
#define ANALYSIS_METHOD_HR_SPECIFICITY		IDM_ROC_HR_SPECIFICITY
#define ANALYSIS_METHOD_SONG_FOM_A1		IDM_ROC_SONG_A1_AUC
#define ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY		IDM_ROC_SONG_A1_SENSITIVITY
#define ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY		IDM_ROC_SONG_A1_SPECIFICITY
#define ANALYSIS_METHOD_SONG_FOM_A2		IDM_ROC_SONG_A2_AUC
#define ANALYSIS_METHOD_SONG_FOM_A2_SENSITIVITY		IDM_ROC_SONG_A2_SENSITIVITY
#define ANALYSIS_METHOD_SONG_FOM_A2_SPECIFICITY		IDM_ROC_SONG_A2_SPECIFICITY



#define ANALYSIS_METHOD_JAFROC1		IDM_FROC_JAFROC1
#define ANALYSIS_METHOD_JAFROC1_WEIGHTED		IDM_FROC_JAFROC1_WEIGHTED
#define ANALYSIS_METHOD_JAFROC		IDM_FROC_JAFROC2
#define ANALYSIS_METHOD_JAFROC_WEIGHTED		IDM_FROC_JAFROC2_WEIGHTED
#define ANALYSIS_METHOD_MAX_NLF		IDM_FROC_MAX_NLF_ANALYSIS
#define ANALYSIS_METHOD_MAX_LLF		IDM_FROC_MAX_LLF_ANALYSIS
#define ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY		IDM_EXP_TRNSFM_SPECIFICITY
#define ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS IDM_FROC_CUSTOM_ANALYSIS


#define ANALYSIS_METHOD_ROI		IDM_ROI


#define ANALYSIS_METHOD_SONG_FOM_A0		120




#define FIT_METHOD_SIMPLEX		210		// GSL
#define FIT_METHOD_SIMAN		211
#define FIT_METHOD_HYBRID		212
#define FIT_METHOD_AMEBSA		213		// test code
#define FIT_METHOD_AMOEBA		214

// select the fitting method
#define FIT_METHOD	FIT_METHOD_AMOEBA
//#define FIT_METHOD	FIT_METHOD_HYBRID

#define FIT_CURVE_ROC			291
#define FIT_CURVE_AFROC			292
#define FIT_CURVE_FROC			293



// Sample Size Estimation
#define MAX_NCASES_80	2000
#define MIN_NCASES_80	20
//#define DESIRED_POWER	0.8
#define MAX_SS_READERS	100


// average raw froc plot points
#define N_AVG_RAW_PTS 200
extern int plot_color_table[ 12 ][ 3 ] ;


// JAFROCDlg.cpp
extern int analysis_method ;
extern bool perform_bootstrap ;
extern CString datafilename, outputfilename, fileName, outfilename;

void GetTimeStamp( CString *timeStamp ) ;
void WriteToConsole( char *str ) ;



// sub-headers by category
#include "Analysis.h"
#include "Bin and Cumulate.h"
#include "IDL Conversion Support.h"
#include "Report.h"
#include "Search Model Fitting.h"
#include "Support Functions.h"
#include "data_handler.h"
#include "Raw FROC.h"
#include "Sample Size Estimation.h"

#include "JAFROC_CORE.h"
#include "ANOVA_DLL.h"
