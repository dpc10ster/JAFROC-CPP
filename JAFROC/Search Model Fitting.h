




// chisqr.cpp
float chisqr(int flag, float mu, float lambda, float nu, int R, float * zeta) ;
float chisqr_ROC(float mu, float lambda, float nu, int R, float * zeta) ;
float chisqr_FROC(int flag, float mu, float lambda, float nu, int R, float * zeta) ;


// constrained_fit.cpp
extern int FP_sm_fit[ 20 ], NL_sm_fit[ 20 ], LL_sm_fit[ 20 ], **TP_sm_fit ;
extern int *N_Lesions_sm_fit, Total_Lesions_sm_fit, n_parameters_sm_fit, R_sm_fit ;
extern int MAX_L_sm_fit, *MAX_CASES_LL_sm_fit ;
extern int error_sm_fit ;

bool constrained_fit_roc( int *fp_bin, int **tp_bin, float *mu, float *chisqr, float *parameters, int n_parameters, int fit_method ) ;
bool constrained_fit_afroc( int *fp_bin, int *ll_bin, float *mu, float *chisqr, float *parameters, int n_parameters, int fit_method ) ;
bool constrained_fit_froc( int *nl_bin, int *ll_bin, float *mu1, float *chisqr, float *parameters, int n_parameters, int fit_method ) ;



// fom.cpp
float beta_to_nu( float beta, float mu ) ;
float jafroc_fom( float mu, float lambda, float nu ) ;
float SM_AUC_FOM_CONSTANT_LESION( float mu, float lambda, float beta, float s ) ;
float sm_auc_fom( float mu, float lambda, float beta, int *N_LESIONS_PER_IMAGE, int NCASES_ABN ) ;
float sm_auc_gamma( float mu, float lambda, float nu, float gamma, int *N_LESIONS_PER_IMAGE, int NCASES_ABN ) ;
float jafroc_fom_gamma( float mu, float lambda, float nu, float gamma, int *N_LESIONS_PER_IMAGE, int NCASES_ABN ) ;

float sm_auc( float mu, float lambda, float nu, int *N_LESIONS_PER_IMAGE, int NCASES_ABN ) ;


// initial_values.cpp
extern float ***parameters_ROC ;
extern int **n_parameters_ROC ;
extern float ***parameters_AFROC ;
extern int **n_parameters_AFROC ;
extern float ***parameters_FROC ;
extern int **n_parameters_FROC ;

void find_zeta_roc_afroc( float lambda, int *fp_bin, float *zeta, int n_bins ) ;
void init_parameters_roc( void ) ;
void clear_parameters_roc( void ) ;
void initial_values_from_roc_data( void ) ;
void init_parameters_afroc( void ) ;
void clear_parameters_afroc( void ) ;
void initial_values_from_afroc_data( void ) ;
void init_parameters_froc( void ) ;
void clear_parameters_froc( void ) ;
void initial_values_from_froc_data( void ) ;



// multimin.cpp
void multimin( float *parameters, int n_parameters) ;



// siman.cpp
void siman( float *parameters, int n_parms ) ;



// run_amebsa.cpp
int run_amebsa( float *parameters, int n_parameters ) ;



// run_amoeba.cpp
int run_amoeba( float *parameters, int n_parameters ) ;



// sm_afroc_ml_fit.cpp
extern float ***afroc_fit_parms ;
extern int **afroc_fit_errors ;
extern float **afroc_fit_avg_parms ;

void init_afroc_fit_parms( void ) ;
void clear_afroc_fit_parms( void ) ;
bool sm_afroc_ml_fit( char *filename ) ;



// sm_froc_ml_fit.cpp
extern float ***froc_fit_parms ;
extern int **froc_fit_errors ;
extern float **froc_fit_avg_parms ;

void init_froc_fit_parms( void ) ;
void clear_froc_fit_parms( void ) ;
bool sm_froc_ml_fit( char *filename ) ;



// sm_roc_ml_fit.cpp
extern float ***roc_fit_parms ;
extern int **roc_fit_errors ;
extern float **roc_fit_avg_parms ;

void init_roc_fit_parms( void ) ;
void clear_roc_fit_parms( void ) ;
bool sm_roc_ml_fit( char *filename ) ;
