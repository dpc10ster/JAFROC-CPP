


// Data_QC_Check.cpp
bool Data_QC_Check( void ) ;



// Analysis.cpp
extern float Results[ 2 ], DF[ 2 ] ;
extern float *Left, *Right ;
extern float **TRT_CI_CASES ;
extern float **TRT_CI_ALL ;
extern float **TRT_CI_RDRS ;

extern float ***FOM_ARR ;
extern float ***PseudoValues ;
void clear_pseudovalues( void ) ;
void alloc_pseudovalues( void ) ;

//extern float MS_T, MS_R, MS_C, MS_TC, MS_RC, MS_TR, MS_TRC ;
extern float var_tr, var_tc, var_trc ;

extern float **TR_MEANS ;
void clear_treatment_means( void ) ;
void alloc_treatment_means( void ) ;


void Alloc_DLL_Memory( void ) ;
void jackknife_FROC_Data_DLL( void ) ;
void jackknife_FROC_Data_Normals_DLL( void ) ;
void __cdecl jackknife_FROC_Data_Abnormals_DLL( void ) ;
