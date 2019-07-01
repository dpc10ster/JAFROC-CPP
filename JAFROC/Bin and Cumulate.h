


// bin_and_cumulate_afroc_data.cpp
extern int ***NL_BIN_AFROC ;
extern int ***LL_BIN_AFROC ;
extern int ***FP_BIN_AFROC ;
extern int ***TP_BIN_AFROC ;
extern float ***NLF_BIN_AFROC ;
extern float ***LLF_BIN_AFROC ;
extern float ***FPF_BIN_AFROC ;
extern float ***TPF_BIN_AFROC ;

void init_binned_afroc_data( void ) ;
void clear_binned_afroc_data( void ) ;
void bin_and_cumulate_afroc_data( void ) ;



// bin_and_cumulate_roc_data.cpp
extern int ***NL_BIN_ROC ;
extern int ***LL_BIN_ROC ;
extern int ***FP_BIN_ROC ;
extern int ****TP_BIN_ROC ;
extern int ***TP_BIN_ROC1 ;
extern float ***NLF_BIN_ROC ;
extern float ***LLF_BIN_ROC ;
extern float ***FPF_BIN_ROC ;
extern float ***TPF_BIN_ROC ;

void init_binned_roc_data( void ) ;
void clear_binned_roc_data( void ) ;
void bin_and_cumulate_roc_data( void ) ;



// bin_and_cumulate_froc_data.cpp
extern int ***NL_BIN_FROC ;
extern int ***LL_BIN_FROC ;
extern int ***FP_BIN_FROC ;
extern int ***TP_BIN_FROC ;
extern float ***NLF_BIN_FROC ;
extern float ***LLF_BIN_FROC ;
extern float ***FPF_BIN_FROC ;
extern float ***TPF_BIN_FROC ;

void init_binned_froc_data( void ) ;
void clear_binned_froc_data( void ) ;
void bin_and_cumulate_froc_data( void ) ;


// convert_to_roc.cpp
extern float ***FP, ***TP ;

void init_roc_data( void ) ;
void clear_roc_data( void ) ;
void convert_to_afroc( void ) ;
void convert_to_roc( void ) ;


// find_optimal_cutoffs.cpp
extern float ***zeta_array_ROC ;
extern int **number_of_bins_ROC ;
extern float ***zeta_array_AFROC ;
extern int **number_of_bins_AFROC ;
extern float ***zeta_array_FROC ;
extern int **number_of_bins_FROC ;

void common_cutoff_routine( float *bad, int bad_len, float *good, int good_len, float *zeta, int *zeta_len ) ;
void init_zeta_roc( void ) ;
void clear_zeta_roc( void ) ;
void find_optimal_cutoffs_roc( void ) ;
void init_zeta_afroc( void ) ;
void clear_zeta_afroc( void ) ;
void find_optimal_cutoffs_afroc( void ) ;
void init_zeta_froc( void ) ;
void clear_zeta_froc( void ) ;
void find_optimal_cutoffs_froc( void ) ;
