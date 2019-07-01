#ifndef __JAFROC_CORE
#define __JAFROC_CORE





#ifdef __cplusplus
extern "C" {
#endif

// anova.cpp
void anova_random_all( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues,
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_ALL
					  ) ;

void anova_random_cases( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues, 
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_CASES
						) ;

void anova_random_rdrs( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues,
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_RDRS
					   ) ;

void __cdecl one_reader_anova( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues, 
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI_CASES
					  ) ;

#if 0

// jackknife_FROC_Data
int jackknife_FROC_Data( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Image, float **weights,
	int analysis_method,
	float ***FOM_ARR, float ***PseudoValues ) ;

int jackknife_FROC_Data_Normals( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Image, float **weights,
	int analysis_method,
	float ***FOM_ARR, float ***PseudoValues ) ;

int jackknife_FROC_Data_Abnormals( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Image, float **weights,
	int analysis_method,
	float ***FOM_ARR, float ***PseudoValues ) ;

#endif

// mean_squares.cpp
void mean_squares(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues,
	float *OUT_MS_T, float *OUT_MS_R, float *OUT_MS_C, float *OUT_MS_TC, float *OUT_MS_RC, float *OUT_MS_TR, float *OUT_MS_TRC,
	float *OUT_var_tr, float *OUT_var_tc, float *OUT_var_trc
	) ;



#ifdef __cplusplus
}
#endif

#endif