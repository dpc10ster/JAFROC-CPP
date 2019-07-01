

extern float es_roc ;
extern float rocfom_nh, rocfom_ah ;
extern float jafrocfom_nh, jafrocfom_ah ;
extern float ss_alpha ;
extern float desired_power ;

extern int ssd_success ;

extern int *sample_size_modality ;
extern int **sample_size_arr ;

extern float nh_roc_mu, nh_roc_lambda, nh_roc_nu ;
extern float ah_roc_mu, ah_roc_lambda, ah_roc_nu ;



void init_sample_size_arr( void ) ;
void clear_sample_size_arr( void ) ;

int SampleSizeMultiReader( void ) ;
int SampleSizeSingleReader( void ) ;

void SampleSizeReport( FILE *stream ) ;
bool SampleSizeGraph( char *basename ) ;