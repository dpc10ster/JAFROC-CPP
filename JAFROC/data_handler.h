




// data_handler.cpp
extern float ****NL ;
extern float ****LL ;
extern int *N_Lesions_Per_Image ;
extern float **Weights ;
extern int *ModalityArray, *ReaderArray, *CaseArray ;
extern int N_TREATMENTS ;
extern int MAX_READERS ;
extern int MAX_CASES[ 2 ] ;
extern int MAX_LL, MAX_NL ;

void init_data( void ) ;
void clear_data( void ) ;
bool read_jafroc_data( char *filename ) ;

extern char **caseTable, **readerTable, **modalityTable ;
extern char *datafileDescriptionText ;