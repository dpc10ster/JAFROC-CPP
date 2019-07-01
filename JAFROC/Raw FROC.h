




// raw_froc.cpp
extern int max_points_froc ;
extern int **nlf_max_froc ;
extern float ***nlf_raw_froc ;
extern float ***llf_raw_froc ;

extern int max_points_afroc ;
extern int **fpf_max_afroc ;
extern float ***fpf_raw_afroc ;
extern float ***llf_raw_afroc ;

extern int max_points_roc ;
extern int **fpf_max_roc ;
extern float ***fpf_raw_roc ;
extern float ***tpf_raw_roc ;

extern float **nlf_avg_raw_froc ;
extern float **llf_avg_raw_froc ;
extern float *fpf_avg_raw_afroc ;
extern float **llf_avg_raw_afroc ;
extern float *fpf_avg_raw_roc ;
extern float **tpf_avg_raw_roc ;


void init_raw_froc( void ) ;
void clear_raw_froc( void ) ;
void raw_froc_op_points( void ) ;

void init_raw_roc( void ) ;
void clear_raw_roc( void ) ;
void raw_roc_op_points( void ) ;

void init_raw_afroc( void ) ;
void clear_raw_afroc( void ) ;
void raw_afroc_op_points( void ) ;
