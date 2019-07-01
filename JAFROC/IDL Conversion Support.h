




// array_count.cpp
int array_count( int *arr, int len, int cutoff1, int cutoff2 ) ;
int array_count( int **arr, int len1, int len2, int cutoff1, int cutoff2 ) ;
int array_count( int ***arr, int len1, int len2, int len3, int cutoff1, int cutoff2 ) ;
int array_count( int ****arr, int len1, int len2, int len3, int len4, int cutoff1, int cutoff2 ) ;
int array_count( float *arr, int len, float cutoff1, float cutoff2 ) ;
int array_count( float **arr, int len1, int len2, float cutoff1, float cutoff2 ) ;
int array_count( float ***arr, int len1, int len2, int len3, float cutoff1, float cutoff2 ) ;
int array_count( float ****arr, int len1, int len2, int len3, int len4, float cutoff1, float cutoff2 ) ;



// where.cpp
bool where( int *arr, int arrlen, char *exp, int val, int *index, int *retlen ) ;
bool where( int **arr, int len1, int len2, char *exp, float val, int *index, int *retlen ) ;
bool where( int *arr, int arrlen, char *exp, int val, int *index, int *ret, int *retlen ) ;
bool where( int **arr, int len1, int len2, char *exp, float val, int *index, int *ret, int *retlen ) ;
bool where( float *arr, int arrlen, char *exp, float val, int *index, int *retlen ) ;
bool where( float **arr, int len1, int len2, char *exp, float val, int *index, int *retlen ) ;
bool where( float *arr, int arrlen, char *exp, float val, int *index, float *ret, int *retlen ) ;
bool where( float **arr, int len1, int len2, char *exp, float val, int *index, float *ret, int *retlen ) ;



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
