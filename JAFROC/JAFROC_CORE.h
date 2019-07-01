#ifndef __JAFROC_CORE
#define __JAFROC_CORE


#ifdef JAFROC_CORE_EXPORTS
#define JAFROC_CORE_API __declspec(dllexport)
#else
#define JAFROC_CORE_API __declspec(dllimport)
#endif



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


#ifdef __cplusplus
extern "C" {
#endif


// anova.cpp
JAFROC_CORE_API void __cdecl anova( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float **TR_MEANS, float ***PseudoValues, int anova_random,
	float MS_T, float MS_R, float MS_C, float MS_TC, float MS_RC, float MS_TR, float MS_TRC,
	float *Results, float *DF, float *Left, float *Right, float **TRT_CI, int *err
					   ) ;

// jackknife_FROC_Data
JAFROC_CORE_API void __cdecl jackknife_FROC_Data( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Image, 
	int analysis_method, float **weights,
	float **TR_MEANS, float ***PseudoValues, int *err ) ;

JAFROC_CORE_API void __cdecl jackknife_FROC_Data_Normals( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Image, 
	int analysis_method, float **weights,
	float **TR_MEANS, float ***PseudoValues, int *err ) ;


JAFROC_CORE_API void __cdecl jackknife_FROC_Data_Abnormals( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, float ****NL, float ****LL, int *N_Lesions_Per_Image, 
	int analysis_method, float **weights,
	float **TR_MEANS, float ***PseudoValues, int *err ) ;

// mean_squares.cpp
JAFROC_CORE_API void __cdecl mean_squares(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, float ***PseudoValues,
	float *OUT_MS_T, float *OUT_MS_R, float *OUT_MS_C, float *OUT_MS_TC, float *OUT_MS_RC, float *OUT_MS_TR, float *OUT_MS_TRC,
	float *OUT_var_tr, float *OUT_var_tc, float *OUT_var_trc, int *err
	) ;

JAFROC_CORE_API float __cdecl JAFROC_FOM( 
	int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float **NL, float **LL, int *N_Lesions_Per_Image, float **weights,
	int analysis_method, int *err ) ;

// memory allocation
JAFROC_CORE_API void __cdecl Allocate_analysis_memory(
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN,
	float ***TR_MEANS, float ****PseudoValues, float ***TRT_CI
	) ;

JAFROC_CORE_API void __cdecl Free_analysis_memory(
	int N_TREATMENTS, int MAX_READERS,
	float **TR_MEANS, float ***PseudoValues, float **TRT_CI
	) ;


// Excel File reader
JAFROC_CORE_API void __cdecl XLReader( 
	char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL, 
	float *****NL, float *****LL, int **N_Lesions_Per_Case, float ***Weights, 
	char ***readerTable, char ***modalityTable, int *err, char *errorString ) ;

JAFROC_CORE_API void __cdecl JAFROC_V1_Loader( 
	char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL, 
	float *****NL, float *****LL, int **N_Lesions_Per_Case, float ***Weights, 
	char ***readerTable, char ***modalityTable, int *err, char *errorString ) ;

JAFROC_CORE_API void __cdecl Free_FROC_Data( 
	int N_TREATMENTS, int MAX_READERS, int NCASES_NOR, int NCASES_ABN, int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights, 
	char **readerTable, char **modalityTable ) ;

JAFROC_CORE_API void __cdecl Get_JAFROC_CORE_DLL_Timestamp( char *str ) ;

JAFROC_CORE_API void __cdecl ReadJAFROCDataFileDescription( char *filename, char *description ) ;

#ifdef __cplusplus
}
#endif

#endif