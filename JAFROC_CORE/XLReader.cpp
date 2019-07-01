#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"
//#include "../common.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory.h>
#include <stdlib.h>

#include "CSpreadSheet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//extern CStringArray caseTable, readerTable, modalityTable ;
CObArray lesionTable ;

int *data_entry_flag ;

char str[ 200 ] ; 


//int GetMaxNumbers( char *filename )
int GetMaxNumber( char *filename, int *N_TREATMENTS, int *MAX_READERS, int *NCASES_NOR, int *NCASES_ABN, int *MAX_NL, int *MAX_LL )
{

	int i, j, k ;
	int row, col ;
	CStringArray rowvalue ;	
	int maxlesion = 0 ;
//	int lesioni ;
	int casei ;
	int nor = 0, abn = 0 ;
//	char str[ 200 ] ;

	int cfpr = -1, cfpm = -1, cfpc = -1, inc = 0, ofpr = -2, ofpm = -2, ofpc = -2 ;

//	CStringArray caseTable, readerTable, modalityTable, casesTable ;
	CStringArray caseTable, readerTable, modalityTable ;
	CObArray lesionTable ;


	// obtain MAX_CASES from "Truth" worksheet
	CString filenamestr ;
	filenamestr = filename ;
	CSpreadSheet wgt( filenamestr, _T("Truth"), false ) ;	
	if( &wgt == NULL ) return 1 ;

	row = wgt.GetTotalRows() ;
	col = wgt.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;

	// create lesion table and check consistency of the Truth table
	for( j = 2 ; j <= row ; j++ ) {
		wgt.ReadRow( rowvalue, j ) ;

		// blank cell check 
		int nblank = 0 ;
		for( i = 0 ; i < 3 ; i++ ) if( rowvalue.GetAt( i ).IsEmpty() ) nblank ++ ;
		if( nblank > 0 && nblank < 3 ) {
			sprintf( str, "Blank cell detected at row %d in Truth worksheet.", j ) ;
//			WriteToConsole( str ) ;
			return 2 ;	// blank check
		}
		if( !rowvalue.GetAt(0).IsEmpty() ) {
			casei = -1 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					casei = i ;
					((CStringArray*)lesionTable.GetAt( i ))->Add( rowvalue.GetAt( 1 ) ) ;
					break ;
				}
			}
			if( casei == - 1 ) { 
				caseTable.Add( rowvalue.GetAt( 0 ) ) ;
				lesionTable.Add( new CStringArray() ) ;
				((CStringArray*)lesionTable.GetAt( lesionTable.GetSize() - 1 ))->Add( rowvalue.GetAt( 1 ) ) ;
				if( rowvalue.GetAt( 1 ) == CString( "0.0" ) ) nor++ ;
			}
		}
/*
		lesioni = atoi( rowvalue.GetAt( 1 ).GetBuffer( 0 ) ) ;
		if( lesioni > maxlesion ) maxlesion = lesioni ;
		if( lesioni == 0 ) nor ++ ; */
	}
	maxlesion = 0 ;
//	nor = 0 ;
	for( i = 0 ; i < lesionTable.GetSize() ; i++ ) {
		if( ((CStringArray*)lesionTable.GetAt( i ))->GetSize() > maxlesion ) maxlesion = ((CStringArray*)lesionTable.GetAt( i ))->GetSize() ;
//		if( ((CStringArray*)lesionTable.GetAt( i ))->GetAt( 0 ) == CString( "0.0" ) ) nor++ ;
	}

	/*
	MAX_CASES[ 0 ] = nor ;
	MAX_CASES[ 1 ] = caseTable.GetSize() - nor ;
	MAX_LL = maxlesion ;
	*/
	*NCASES_NOR = nor ;
	*NCASES_ABN = caseTable.GetSize() - nor ;
	*MAX_LL = maxlesion ;


	CString fpstr ;
	filenamestr = filename ;
	fpstr = "FP" ;
	CSpreadSheet fp( filenamestr, fpstr, false ) ;	
	if( &fp == NULL ) return 1 ;

	row = fp.GetTotalRows() ;
	col = fp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;


	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		int hit = 0 ;
		fp.ReadRow( rowvalue, j ) ;

		// check blank cell
//		for( i = 0 ; i < 4 ; i++ ) if( rowvalue.GetAt( i ).IsEmpty() ) {
		// temporarily reduced the range because the class CSpreadSheet seems it cannot read string properly
		int nblank = 0 ;
		for( i = 0 ; i < 3 ; i++ ) if( rowvalue.GetAt( i ).IsEmpty() ) nblank ++ ;
		if( nblank > 0 && nblank < 3 ) {
			CString cell ;
			wgt.ReadCell( cell, i+1, j ) ;
			sprintf( str, "Blank cell detected at row %d in FP worksheet.", j ) ;
//			WriteToConsole( str ) ;
			return 2 ;
		}
		if( !( rowvalue.GetAt(0).IsEmpty() || rowvalue.GetAt(1).IsEmpty() || rowvalue.GetAt(2).IsEmpty() ) ) {
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					hit = 1 ;
					cfpr = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				readerTable.Add( rowvalue.GetAt( 0 ) ) ;
				cfpr = readerTable.GetSize() - 1 ;
			}
			hit = 0 ;
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					hit = 1 ;
					cfpm = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				modalityTable.Add( rowvalue.GetAt( 1 ) ) ;
				cfpm = modalityTable.GetSize() - 1 ;
			}
			hit = 0 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 2 ) ) {
					hit = 1 ;
					cfpc = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				// the case number on FP not exist on Truth
				// return with error message
				sprintf( str, "Case ID %s on FP worksheet does not exist on Truth worksheet.", rowvalue.GetAt( 2 ) ) ;
//				WriteToConsole( str ) ;
				return 2 ;
//				casesTable.Add( rowvalue.GetAt( 2 ) ) ;
//				cfpc = casesTable.GetSize() - 1 ;
			}
			if( cfpr != ofpr || cfpm != ofpm || cfpc != ofpc ) {
				if( inc > *MAX_NL ) *MAX_NL = inc ;
				inc = 0 ;
				ofpr = cfpr ;
				ofpm = cfpm ;
				ofpc = cfpc ;
			} else {
				inc++ ;
			}
		}
	}

	// hjy
//	*MAX_NL += 10 ;

	filenamestr = filename ;
	CString tpstr ;
	tpstr = "TP" ;
	CSpreadSheet tp( filenamestr, tpstr, false ) ;	
	if( &tp == NULL ) return 1 ;

	row = tp.GetTotalRows() ;
	col = tp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;


	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		int hit = 0 ;
		tp.ReadRow( rowvalue, j ) ;

		// blank cell check
//		for( i = 0 ; i < 5 ; i++ ) if( rowvalue.GetAt( i ).IsEmpty() ) {
		// temporarily reduced the range because the class CSpreadSheet seems it cannot read string properly
		int nblank = 0 ;
		for( i = 0 ; i < 4 ; i++ ) if( rowvalue.GetAt( i ).IsEmpty() ) nblank ++ ;
		if( nblank > 0 && nblank < 4 ) {
			sprintf( str, "Blank cell detected at row %d in TP worksheet.", j ) ;
//			WriteToConsole( str ) ;
			return 2 ;	// blank check
		}
		if( !( rowvalue.GetAt(0).IsEmpty() || rowvalue.GetAt(1).IsEmpty() ) ) {
			hit = 0 ;
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				readerTable.Add( rowvalue.GetAt( 0 ) ) ;
			}
			hit = 0 ;
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				modalityTable.Add( rowvalue.GetAt( 1 ) ) ;
			}
			hit = 0 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 2 ) ) {
					hit = 1 ;
					cfpc = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				// the case number on TP not exist on Truth
				// return with error message
				sprintf( str, "Case ID %s on TP worksheet does not exist on Truth worksheet.", rowvalue.GetAt( 2 ) ) ;
//				WriteToConsole( str ) ;
				return 2 ;
			} else {
				hit = 0 ;
				for( i = 0 ; i < ((CStringArray*)lesionTable.GetAt(cfpc))->GetSize() ; i++ ) {
					if( ((CStringArray*)lesionTable.GetAt(cfpc))->GetAt( i ) == rowvalue.GetAt( 3 ) ) {
						hit = 1 ;
					}
				}
				if( hit == 0 ) {
					// lesion id does not matched with Truth worksheet
					sprintf( str, "Case ID %s Lesion ID %s is not exist on Truth worksheet.", rowvalue.GetAt( 2 ), rowvalue.GetAt( 3 ) ) ;
//					WriteToConsole( str ) ;
					return 2 ;
				}
			}
		}
	}

	*MAX_READERS = readerTable.GetSize() ;
	*N_TREATMENTS = modalityTable.GetSize() ;


	// get MAX_NL
	int ***NL_Count_Arr = new int **[ modalityTable.GetCount() ] ;
	for( i = 0 ; i < modalityTable.GetCount() ; i++ ) {
		NL_Count_Arr[ i ] = new int *[ readerTable.GetCount() ] ;
		for( j = 0 ; j < readerTable.GetCount() ; j++ ) {
			NL_Count_Arr[ i ][ j ] = new int[ caseTable.GetCount() ] ;
			for( k = 0 ; k < caseTable.GetCount() ; k++ ) {
				NL_Count_Arr[ i ][ j ][ k ] = 0 ;
			}
		}
	}
	*MAX_NL = 0 ;

//	CString fpstr ;
	filenamestr = filename ;
	fpstr = "FP" ;
//	CSpreadSheet fp( filenamestr, fpstr, false ) ;	
	if( &fp == NULL ) return 1 ;

	row = fp.GetTotalRows() ;
	col = fp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;

	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		int hit = 0 ;
		fp.ReadRow( rowvalue, j ) ;

		// check blank cell
		int nblank = 0 ;
		for( i = 0 ; i < 3 ; i++ ) if( rowvalue.GetAt( i ).IsEmpty() ) nblank ++ ;
		if( nblank > 0 && nblank < 3 ) {
			CString cell ;
			wgt.ReadCell( cell, i+1, j ) ;
			sprintf( str, "Blank cell detected at row %d in FP worksheet.", j ) ;
//			WriteToConsole( str ) ;
			return 2 ;
		}
		if( !( rowvalue.GetAt(0).IsEmpty() || rowvalue.GetAt(1).IsEmpty() || rowvalue.GetAt(2).IsEmpty() ) ) {
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					hit = 1 ;
					cfpr = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				return 1 ;
			}
			hit = 0 ;
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					hit = 1 ;
					cfpm = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				return 1 ;
			}
			hit = 0 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 2 ) ) {
					hit = 1 ;
					cfpc = i ;
					break ;
				}
			}
			if( hit == 0 ) {
				return 1 ;
			}
			NL_Count_Arr[ cfpm ][ cfpr ][ cfpc ]++ ;
		}
	}

	// hjy
	*MAX_NL = 0 ;
	for( i = 0 ; i < modalityTable.GetCount() ; i++ ) {
		for( j = 0 ; j < readerTable.GetCount() ; j++ ) {
			for( k = 0 ; k < caseTable.GetCount() ; k++ ) {
				if( NL_Count_Arr[ i ][ j ][ k ] > *MAX_NL ) *MAX_NL = NL_Count_Arr[ i ][ j ][ k ] ;
			}
		}
	}
	for( i = 0 ; i < modalityTable.GetCount() ; i++ ) {
		for( j = 0 ; j < readerTable.GetCount() ; j++ ) {
			delete[] NL_Count_Arr[ i ][ j ] ;
		}
		delete[] NL_Count_Arr[ i ] ;
	}
	delete[] NL_Count_Arr ;

	for( i = 0 ; i < lesionTable.GetSize() ; i++ ) {
		if( lesionTable.GetAt(i) != NULL ) {
			CStringArray *arr = ((CStringArray *)lesionTable.GetAt(i)) ;
			arr->RemoveAll() ;
			delete arr ;
		}
	}
	lesionTable.RemoveAll() ;

	caseTable.RemoveAll() ;
	modalityTable.RemoveAll() ;
	readerTable.RemoveAll() ;

	return 0 ;

}



int ReadTruthTable( char *filename, int *MAX_CASES, CStringArray &caseTable, int *N_Lesions_Per_Image, float **Weights )
{

	int i, j ;
	int row, col ;
	CStringArray rowvalue ;	
	int maxlesion = 0 ;
	int nor = 0, abn = 0 ;
	int casei, lesioni, abnormality ;
	int total_max_cases = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;
	CString rowOb ;

	CString filenamestr, truthstr ;
	filenamestr = filename ;
	truthstr = "Truth" ;
	CSpreadSheet wgt( filenamestr, truthstr, false ) ;	
	if( &wgt == NULL ) return 1 ;

	row = wgt.GetTotalRows() ;
	col = wgt.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;

	lesionTable.SetSize( total_max_cases ) ;
	for( j = 0 ; j < total_max_cases ; j++ ) {
		lesionTable.SetAt( j, NULL ) ;
	}

	for( j = 2 ; j <= row ; j++ ) {
		wgt.ReadRow( rowvalue, j ) ;
		if( !rowvalue.GetAt(0).IsEmpty() ) {
			casei = -1 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					casei = i ;
					break ;
				}
			}
			if( casei == - 1 ) caseTable.Add( rowvalue.GetAt( 0 ) ) ;
		}
	}

	for( j = 2 ; j <= row ; j++ ) {
		wgt.ReadRow( rowvalue, j ) ;
		if( !rowvalue.GetAt(0).IsEmpty() ) {
			casei = -1 ;
			abnormality = 1 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					casei = i ;
					if( rowvalue.GetAt( 1 ) == CString( "0.0" ) ) abnormality = 0 ;
					break ;
				}
			}
			if( abnormality == 0 ) {
				N_Lesions_Per_Image[ casei ] = 0 ;
			} else {
				if( lesionTable.GetAt( casei ) == NULL ) {
					lesionTable.SetAt( casei, new CStringArray() ) ;
				}
				((CStringArray *)lesionTable.GetAt( casei ))->Add( rowvalue.GetAt( 1 ) ) ;
				lesioni = ((CStringArray *)lesionTable.GetAt( casei ))->GetSize() ;
				N_Lesions_Per_Image[ casei ] = lesioni ;
				rowOb = rowvalue.GetAt( 2 ) ;
				Weights[ casei ][ lesioni - 1 ] = (float)atof( (char *)rowOb.GetBuffer( 0 ) ) ;
			}
		}
	}

	for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
		if( N_Lesions_Per_Image[ i ] == 0 ) nor++ ;
		else abn++ ;
	}

	return 0 ;

}



int CreateTables( char *fpname, CStringArray &readerTable, CStringArray &modalityTable )
{

	int i, j ;
	int row, col ;
	CStringArray rowvalue ;	

	CString filenamestr, fpstr ;
	filenamestr = fpname ;
	fpstr = "FP" ;
	CSpreadSheet fp( filenamestr, fpstr, false ) ;	
	if( &fp == NULL ) return 1 ;

	row = fp.GetTotalRows() ;
	col = fp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;


	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		fp.ReadRow( rowvalue, j ) ;
		int hit = 0 ;
		if( !rowvalue.GetAt(0).IsEmpty() ) {
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				readerTable.Add( rowvalue.GetAt( 0 ) ) ;
			}
		}
		hit = 0 ;
		if( !rowvalue.GetAt(1).IsEmpty() ) {
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				modalityTable.Add( rowvalue.GetAt( 1 ) ) ;
			}
		}
	}

	CString tpstr ;
	tpstr = "TP" ;
	CSpreadSheet tp( filenamestr, tpstr, false ) ;	
	if( &tp == NULL ) return 1 ;

	row = tp.GetTotalRows() ;
	col = tp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;


	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		tp.ReadRow( rowvalue, j ) ;
		int hit = 0 ;
		if( !rowvalue.GetAt(0).IsEmpty() ) {
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				readerTable.Add( rowvalue.GetAt( 0 ) ) ;
			}
		}
		hit = 0 ;
		if( !rowvalue.GetAt(1).IsEmpty() ) {
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					hit = 1 ;
					break ;
				}
			}
			if( hit == 0 ) {
				modalityTable.Add( rowvalue.GetAt( 1 ) ) ;
			}
		}
	}

	return 0 ;

}



int ReadFP( char *filename, int MAX_CASES[ 2 ], int MAX_NL, CStringArray &readerTable, CStringArray &modalityTable, CStringArray &caseTable, float ****NL )
{

	int i, j ;
	int row, col ;
	int ir, im, ic, irt ;
	CStringArray rowvalue ;	
	int total_max_cases = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;

	CString filenamestr, fpstr ;
	filenamestr = filename ;
	fpstr = "FP" ;
	CSpreadSheet fp( filenamestr, fpstr, false ) ;	
	if( &fp == NULL ) return 1 ;

	row = fp.GetTotalRows() ;
	col = fp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;

	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		fp.ReadRow( rowvalue, j ) ;
		if( !( rowvalue.GetAt(0).IsEmpty() || rowvalue.GetAt(1).IsEmpty() || rowvalue.GetAt(2).IsEmpty() || rowvalue.GetAt(3).IsEmpty() ) ) {
			ir = -1 ;
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					ir = i ;
					break ;
				}
			}
			im = -1 ;
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					im = i ;
					break ;
				}
			}
			ic = - 1 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 2 ) ) {
					ic = i ;
					break ;
				}
			}
			irt = -1 ;
			for( i = 0 ; i < MAX_NL ; i++ ) {
				if( NL[ im ][ ir ][ ic ][ i ] == UNINITIALIZED ) {
					irt = i ;
					break ;
				}
			}
			if( irt == -1 ) {
				return 1 ;
			}
			if( ir != -1 && im != -1 && ic != -1 && irt != -1 ) {
				NL[ im ][ ir ][ ic ][ irt ] = (float)atof( rowvalue.GetAt(3) ) ;
			}
		}
	}

	return 0 ;

}


int ReadTP( char *filename, int MAX_CASES[ 2 ], CStringArray &readerTable, CStringArray &modalityTable, CStringArray &caseTable, float ****LL )
{

	int i, j ;
	int row, col ;
	int ir, im, ic, irt ;
	CStringArray rowvalue ;	
	int total_max_cases = MAX_CASES[ 0 ] + MAX_CASES[ 1 ] ;
//	char str[ 200 ] ;

	CString filenamestr, tpstr ;
	filenamestr = filename ;
	tpstr = "TP" ;
	CSpreadSheet tp( filenamestr, tpstr, false ) ;	
	if( &tp == NULL ) return 1 ;

	row = tp.GetTotalRows() ;
	col = tp.GetTotalColumns() ;

	if( row == 0 || col == 0 ) return 1 ;

	// scan tables 
	for( j = 2 ; j <= row ; j++ ) {
		tp.ReadRow( rowvalue, j ) ;
		if( !( rowvalue.GetAt(0).IsEmpty() || rowvalue.GetAt(1).IsEmpty() || rowvalue.GetAt(2).IsEmpty() || rowvalue.GetAt(3).IsEmpty() || rowvalue.GetAt(4).IsEmpty() ) ) {
			ir = -1 ;
			for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
				if( readerTable.GetAt( i ) == rowvalue.GetAt( 0 ) ) {
					ir = i ;
					break ;
				}
			}
			im = -1 ;
			for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
				if( modalityTable.GetAt( i ) == rowvalue.GetAt( 1 ) ) {
					im = i ;
					break ;
				}
			}
			ic = - 1 ;
			for( i = 0 ; i < caseTable.GetSize() ; i++ ) {
				if( caseTable.GetAt( i ) == rowvalue.GetAt( 2 ) ) {
					ic = i ;
					break ;
				}
			}
			if( ir != -1 && im != -1 && ic != -1 ) {
				irt = -1 ;
				if( (CStringArray *)lesionTable.GetAt( ic ) == NULL ) {
					irt = 1 ;
				}
				for( i = 0 ; i < ((CStringArray *)lesionTable.GetAt( ic ))->GetSize() ;  i++ ) {
					if( ((CStringArray *)lesionTable.GetAt( ic ))->GetAt( i ) == rowvalue.GetAt( 3 ) ) {
						irt = i ;
					}
				}
				if( irt == -1 ) {
					CString rowOb1, rowOb2 ;
					rowOb1 = rowvalue.GetAt( 3 ) ;
					rowOb2 = rowvalue.GetAt( 2 ) ;
					sprintf( str, "LesionID %s in Case %s not exist in Truth worksheet. Check Row %d in TP worksheet.", 
						rowOb1.GetBuffer(0), rowOb2.GetBuffer(0),j ) ;
//					WriteToConsole( str ) ;
					return 1 ;
				}
				if( LL[ im ][ ir ][ ic ][ irt ] != UNINITIALIZED ) {
					// entry duplication error
//					char str[ 200 ] ;
					sprintf( str, "There are two True Positive entries for the same Reader, Modality, Case and Lesion. Check Row %d in TP worksheet.",
						j ) ;
//					WriteToConsole( str ) ;
					return 1 ;
				}
				
				LL[ im ][ ir ][ ic ][ irt ] = (float)atof( rowvalue.GetAt( 4 ) ) ;
			}
		}
	}

	return 0 ;

}



int ReadFROCDataXLS( 
	char *filename, int N_TREATMENTS, int MAX_READERS, int MAX_CASES[ 2 ], int MAX_NL, int MAX_LL, 
	float ****NL, float ****LL, int *N_Lesions_Per_Case, float **Weights, 
	CStringArray &caseTable, CStringArray &readerTable, CStringArray &modalityTable )
{
	int i, ret ;

	if( readerTable.GetSize() != 0 ) {
		caseTable.RemoveAll() ;
		readerTable.RemoveAll() ;
		modalityTable.RemoveAll() ;
	}

	// Read Truth Table first
	ret = ReadTruthTable( filename, MAX_CASES, caseTable, N_Lesions_Per_Case, Weights ) ;
	if( ret != 0 ) return 1 ;

	ret = CreateTables( filename, readerTable, modalityTable ) ;
	if( ret != 0 ) return 1 ;
/*
	for( i = 0 ; i < readerTable.GetSize() ; i++ ) ReaderArray[ i ] = i ;
	for( i = 0 ; i < modalityTable.GetSize() ; i++ ) ModalityArray[ i ] = i ;
	for( i = 0 ; i < caseTable.GetSize() ; i++ ) CaseArray[ i ] = i ;
*/
	ret = ReadFP( filename, MAX_CASES, MAX_NL, readerTable, modalityTable, caseTable, NL ) ;
	if( ret != 0 ) return 1 ;

	ret = ReadTP( filename, MAX_CASES, readerTable, modalityTable, caseTable, LL ) ;
	if( ret != 0 ) return 1 ;

	for( i = 0 ; i < lesionTable.GetSize() ; i++ ) {
		if( lesionTable.GetAt(i) != NULL ) {
			CStringArray *arr = ((CStringArray *)lesionTable.GetAt(i)) ;
			arr->RemoveAll() ;
			delete arr ;
		}
	}
	lesionTable.RemoveAll() ;

	for( i = 0 ; i < modalityTable.GetSize() ; i++ ) {
//		if( modalityTable[ i ].Find( '.' ) != -1 ) {
		int isnum = 1 ;
		for( int j = 0 ; j < strlen( modalityTable[ i ] ) ; j++ ) if( modalityTable[ i ].GetAt( j ) > 0x40 && modalityTable[ i ].GetAt( j ) != '.' ) isnum = 0 ;
		if( isnum == 1 ) {
			int val = (int)atof( modalityTable[ i ] ) ;
			modalityTable[ i ].Format( _T("%d"), val ) ;
		}
	}

	for( i = 0 ; i < readerTable.GetSize() ; i++ ) {
		int isnum = 1 ;
		for( int j = 0 ; j < strlen( readerTable[ i ] ) ; j++ ) if( readerTable[ i ].GetAt( j ) > 0x40 && readerTable[ i ].GetAt( j ) != '.' ) isnum = 0 ;
		if( isnum == 1 ) {
			int val = (int)atof( readerTable[ i ] ) ;
			readerTable[ i ].Format( _T("%d"), val ) ;
		}
	}

	return 0 ;
}


extern "C" JAFROC_CORE_API void __cdecl ReadJAFROCDataFileDescription( char *filename, char *description )
{
	CString filenamestr, sheetstr ;
	filenamestr = filename ;
	sheetstr = "Summary" ;
	CSpreadSheet desc( filenamestr, sheetstr, false ) ;	
	if( &desc == NULL ) {
		description[ 0 ] = 0 ;
		return ;
	}

	int row = desc.GetTotalRows() ;
	int col = desc.GetTotalColumns() ;

	if( row == 0 || col == 0 ) {
		description[ 0 ] = 0 ;
		return ;
	}

	CStringArray rowvalue ;	
	char *desc_ptr = description ;
	desc_ptr[ 0 ] = 0 ;

	for( int j = 1 ; j <= row ; j++ ) {
		desc.ReadRow( rowvalue, j ) ;
		if( !rowvalue.GetAt( 0 ).IsEmpty() ) {
			for( int i = 0 ; i < rowvalue.GetSize() ; i++ ) {
				CString str = rowvalue.GetAt( i ) ;
				strcat( desc_ptr, str ) ;
				desc_ptr += strlen( str ) ;
				desc_ptr[ 0 ] = ' ' ;
				desc_ptr++ ;
				desc_ptr[ 0 ] = 0 ;
			}
		}
		strcat( desc_ptr, "\r\n" ) ;
		desc_ptr += 2 ;
		desc_ptr[ 0 ] = 0 ;
	}
//	desc_ptr[ 0 ] = 0 ;
}
