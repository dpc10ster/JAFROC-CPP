// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the JAFROC_CORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// JAFROC_CORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef JAFROC_CORE_EXPORTS
#define JAFROC_CORE_API __declspec(dllexport)
#else
#define JAFROC_CORE_API __declspec(dllimport)
#endif
/*
// This class is exported from the JAFROC_CORE.dll
class JAFROC_CORE_API CJAFROC_CORE {
public:
	CJAFROC_CORE(void);
	// TODO: add your methods here.
};

extern JAFROC_CORE_API int nJAFROC_CORE;

JAFROC_CORE_API int fnJAFROC_CORE(void);
*/

#define	UNINITIALIZED	-2000.0f



#define ANOVA_RANDOM_ALL			200
#define ANOVA_RANDOM_CASES			201
#define ANOVA_RANDOM_READERS		202

#define JAFROC_DLL_ERROR_ZERO_N_ABN		0x01
#define JAFROC_DLL_ERROR_ZERO_N_NOR		0x02
#define JAFROC_DLL_ERROR_ZERO_N_MAX_NL	0x04
#define JAFROC_DLL_ERROR_ZERO_N_MAX_LL	0x08

#define JAFROC_DLL_ERROR_ZERO_READER	0x10
#define JAFROC_DLL_ERROR_ZERO_TREATMENT	0x20
#define JAFROC_DLL_ERROR_INVALID_METHOD	0x40
#define JAFROC_DLL_ERROR_FLOAT_ERROR	0x80

#define JAFROC_DLL_ERROR_INVALID_ANOVA	0x0100
#define JAFROC_DLL_ERROR_XL_FILE_READ	0x0200


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



float MyFOM( float **NL_ij, float **LL_ij, int *n_lesions_per_image, int *max_cases, int max_nl, int max_ll, float **weights, int analysis_method ) ;
bool calculate_treatment_means(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Image, float **weights,
	int analysis_method,
	float **TR_MEANS, int *err ) ;


// mean_squares.cpp
float mean_pseudovalues_i( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int j, int k ) ;
float mean_pseudovalues_j( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int i, int k ) ;
float mean_pseudovalues_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int i, int j ) ;
float mean_pseudovalues_j_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int i ) ;
float mean_pseudovalues_i_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int j ) ;
float mean_pseudovalues_i_j( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues, int k ) ;
float mean_pseudovalues_i_j_k( int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues ) ;


// AnalyticFormulae.cpp
float gauss_pdf( float x ) ;
float gauss_cvf( float p ) ;
float ZtoP( float z ) ;
float PtoZ( float p ) ;
float Obs_Power( float p_value ) ;
float fpf_poi( float zeta, float lambda ) ;
float nlf_poi( float zeta, float lambda ) ;
float llf_poi( float zeta, float mu, float nu ) ;
float inv_nlf_poi( float nlf, float lambda ) ;
float inv_llf_poi( float llf, float mu, float nu ) ;
//float tpf_poi_constant_lesion( float z, float mu, float lambda, float nu, float s ) ;
//float tpf_poi( float z, float mu, float lambda, float nu ) ;
//float inv_tpf_poi( float target_tpf, float mu, float lambda, float nu ) ;
//float inv_fpf_poi( float target_fpf, float lambda ) ;


// chisqr_pdf.cpp
float chisqr_pdf( float x, float df ) ;
double chisqr_pdf( double x, double df ) ;



// stud_t_dist.cpp
float f_pdf( float x, float dfn, float dfd ) ;
float bisect_pdf( float a[ 2 ], float u, float l ) ;
float t_cvf( float a1, float df ) ;
float t_pdf( float v, float df ) ;



// studrange.cpp
float studrange( float Q, float V, float R ) ;
float inv_studrange( float alpha, float df, float treatments ) ;



// min_max.cpp
int minimum( int *arr, int len ) ;
int minimum( int **arr, int len1, int len2 ) ;
int minimum( int ***arr, int len1, int len2, int len3 ) ;
int minimum( int ****arr, int len1, int len2, int len3, int len4 ) ;
float minimum( float *arr, int len ) ;
float minimum( float **arr, int len1, int len2 ) ;
float minimum( float ***arr, int len1, int len2, int len3 ) ;
float minimum( float ****arr, int len1, int len2, int len3, int len4 ) ;
int maximum( int *arr, int len ) ;
int maximum( int **arr, int len1, int len2 ) ;
int maximum( int ***arr, int len1, int len2, int len3 ) ;
int maximum( int ****arr, int len1, int len2, int len3, int len4 ) ;
float maximum( float *arr, int len ) ;
float maximum( float **arr, int len1, int len2 ) ;
float maximum( float ***arr, int len1, int len2, int len3 ) ;
float maximum( float ****arr, int len1, int len2, int len3, int len4 ) ;
float mean( int *arr, int len ) ;
float mean( int **arr, int len1, int len2 ) ;
float mean( float *arr, int len ) ;
float mean( float **arr, int len1, int len2 ) ;
int total( int *arr, int len ) ;
int total( int **arr, int len1, int len2 ) ;
int total( int ***arr, int len1, int len2, int len3 ) ;
float total( float *arr, int len ) ;
float total( float **arr, int len1, int len2 ) ;
float total( float ***arr, int len1, int len2, int len3 ) ;
float stdev( int *arr, int len ) ;
float stdev( float *arr, int len ) ;



int GetMaxNumber( char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL ) ;
int ReadFROCDataXLS( 
	char *filename, int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights, 
	CStringArray &caseTable, CStringArray &readerTable, CStringArray &modalityTable ) ;


void init_data( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float *****NL, float *****LL, int **N_Lesions_Per_Image, float ***Weights ) ;
void clear_data_temp( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights ) ;
void clear_data( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights ) ;
int organize_cases( int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, float *****NL, float *****LL, int **N_Lesions_Per_Image, float ***Weights ) ;