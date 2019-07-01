




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
float tpf_poi_constant_lesion( float z, float mu, float lambda, float nu, float s ) ;
float tpf_poi( float z, float mu, float lambda, float nu ) ;
float inv_tpf_poi( float target_tpf, float mu, float lambda, float nu ) ;
float inv_fpf_poi( float target_fpf, float lambda ) ;
float tpf_poi_gamma( float z, float mu, float lambda, float nu, float gamma ) ;
float f_cvf( float p, float dfn, float dfd ) ;



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


// myqromb.cpp
double myqromb( double func(double), double a, double b) ;