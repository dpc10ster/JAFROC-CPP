// RawFROCPlotDialog.cpp : implementation file
//

#include "stdafx.h"
#include "JAFROC.h"
#include "RawFROCPlotDialog.h"
#include "common.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MODE_RAW_ROC	10
#define MODE_RAW_AFROC	12
#define MODE_RAW_FROC	14



extern CString selectedfilename, datafilename, outputfilename ;


// modality color table
int plot_color_table[ 12 ][ 3 ] = {
	{ 192,   0,   0, },
	{   0, 192,   0, },
	{   0,   0, 192, }, 
	{ 192, 192,   0, },
	{   0, 192, 192, },
	{ 192,   0, 192, }, 
	{ 128,   0,   0, },
	{   0, 128,   0, },
	{   0,   0, 128, }, 
	{ 128, 128,   0, },
	{   0, 128, 128, },
	{ 128,   0, 128, }, 
} ;


int raw_plot_first ;
int plot_reader ;
int *plot_modality_arr = NULL ;

// CRawFROCPlotDialog dialog

IMPLEMENT_DYNAMIC(CRawFROCPlotDialog, CDialog)

CRawFROCPlotDialog::CRawFROCPlotDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRawFROCPlotDialog::IDD, pParent)
{
	plot_reader = 0 ;
	raw_plot_first = 1 ;

	if( plot_modality_arr != NULL ) delete[] plot_modality_arr ;
	plot_modality_arr = new int[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		plot_modality_arr[ i ] = 0 ;
	}
	plot_modality_arr[ 0 ] = 1 ;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;
	SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
}

CRawFROCPlotDialog::~CRawFROCPlotDialog()
{
	if( plot_modality_arr != NULL ) {
		delete[] plot_modality_arr ;
		plot_modality_arr = NULL ;
	}
}

void CRawFROCPlotDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RAW_PLOT_READER, m_list_reader);
	DDX_Control(pDX, IDC_RAW_PLOT_MODALITY, m_list_modality);
}


BEGIN_MESSAGE_MAP(CRawFROCPlotDialog, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RAW_FROC_PREV, &CRawFROCPlotDialog::OnBnClickedRawFrocPrev)
	ON_BN_CLICKED(IDC_RAW_FROC_NEXT, &CRawFROCPlotDialog::OnBnClickedRawFrocNext)
	ON_BN_CLICKED(IDC_SAVE_CSV, &CRawFROCPlotDialog::OnBnClickedSaveCsv)
	ON_LBN_SELCHANGE(IDC_RAW_PLOT_READER, &CRawFROCPlotDialog::OnLbnSelchangeRawPlotReader)
	ON_LBN_SELCHANGE(IDC_RAW_PLOT_MODALITY, &CRawFROCPlotDialog::OnLbnSelchangeRawPlotModality)
END_MESSAGE_MAP()


// CRawFROCPlotDialog message handlers

void CRawFROCPlotDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	char str[ 200 ] ;

	RECT rect ;
	GetClientRect( &rect ) ;
	rect.left = rect.right / 2 ;
	rect.left += 5 ;
	rect.right -= 10 ;
	rect.top += 10 ;
	rect.bottom /= 2 ;
	rect.bottom -= 5 ;

	PlotIndividualCurve( dc, rect, MODE_RAW_ROC ) ;

	GetClientRect( &rect ) ;
	rect.left = rect.right / 2 ;
	rect.left += 5 ;
	rect.right -= 10 ;
	rect.top = rect.bottom / 2 ;
	rect.top += 5 ;
	rect.bottom -= 10 ;

	PlotIndividualCurve( dc, rect, MODE_RAW_AFROC ) ;

	GetClientRect( &rect ) ;
	rect.left += 10 ;
	rect.right /= 2 ;
	rect.right -= 5 ;
	rect.top = rect.bottom / 2 ;
	rect.top += 5 ;
	rect.bottom -= 10 ;

	PlotIndividualCurve( dc, rect, MODE_RAW_FROC ) ;

	if( raw_plot_first ) {
		raw_plot_first = 0 ;

		if( MAX_READERS > 1 ) m_list_reader.AddString( "Average" ) ;
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			m_list_reader.AddString( readerTable[ j ] ) ;
		}
		if( MAX_READERS > 1 ) m_list_reader.SetCurSel( 1 ) ;
		else m_list_reader.SetCurSel( 0 ) ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
//			m_list_modality.AddString( modalityTable[ i ] ) ;
			m_list_modality.AddItem( modalityTable[ i ], NULL ) ;
		}
		m_list_modality.SetSel( 0 ) ;
	}

	/*
	RECT rect ;
	GetClientRect( &rect ) ;
	rect.left += 30 ;
	rect.right -= 40 ;
	rect.top += 40 ;
	rect.bottom -= 50 ;

//	SelectObject( dc.GetSafeHdc(), (HBRUSH)GetStockObject( WHITE_BRUSH ) ) ;
	FillRect( dc.GetSafeHdc(), &rect, (HBRUSH)GetStockObject( WHITE_BRUSH ) ) ;
	dc.SetBkMode( TRANSPARENT ) ;

//	dc.TextOutA( rect.left + 300, rect.bottom - 15, "NLF", 3 ) ;

	{
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" );
		lgFnt.lfHeight = 16 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		 
		// Set this font to DC     
		dc.SelectObject( &NewFont );     
		dc.TextOutA( rect.left + 300, rect.bottom - 15, "NLF", 3 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	{
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" );
		lgFnt.lfHeight = 16 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Give rotation as multiples of 10, so rotation will be 90 degrees
		lgFnt.lfOrientation = 900 ;
		lgFnt.lfEscapement =  900 ; 
		 
		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		//	const int DcRestorePoint2 = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		 
		// Set this font to DC     
		dc.SelectObject( &NewFont );     
		dc.TextOutA( rect.left, rect.bottom - 180, "LLF", 3 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	rect.left += 10 ;
	rect.bottom -= 10 ;

	// axis
	SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( BLACK_PEN ) ) ;
	dc.MoveTo( rect.left, rect.bottom - 10 ) ; dc.LineTo( rect.right, rect.bottom - 10 ) ;
	dc.MoveTo( rect.right - 3, rect.bottom - 15 ) ;	dc.LineTo( rect.right, rect.bottom - 10 ) ;	dc.LineTo( rect.right - 3, rect.bottom - 5 ) ;
	dc.MoveTo( rect.left + 10, rect.bottom ) ;	dc.LineTo( rect.left + 10, rect.top ) ;
	dc.MoveTo( rect.left + 5, rect.top + 3 ) ;	dc.LineTo( rect.left + 10, rect.top ) ;	dc.LineTo( rect.left + 15, rect.top + 3 ) ;
	dc.MoveTo( rect.left + 5, rect.top + 10 ) ; dc.LineTo( rect.left + 15, rect.top + 10 ) ;
	dc.MoveTo( rect.right - 10, rect.bottom - 15 ) ; dc.LineTo( rect.right - 10, rect.bottom - 5 ) ;
	dc.TextOut( rect.left + 20, rect.top, "1.0", 3 ) ;
	sprintf_s( str, "%3.1f", nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max[ plot_modality ][ plot_reader ] - 1 ] ) ;
	dc.TextOut( rect.right - 20, rect.bottom - 30, str, strlen(str) ) ;

	dc.MoveTo( rect.left + 10, rect.bottom - 10 ) ;
	for( int i = 0 ; i < nlf_max[ plot_modality ][ plot_reader ] ; i++ ) {
		dc.LineTo( 
			(int)( nlf_raw_froc[ plot_modality ][ plot_reader ][ i ] 
		/ nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left - 20 ) ) + rect.left + 10,
			-(int)( llf_raw_froc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top - 20 ) ) + rect.bottom - 10 ) ;
	}
	if( nlf_max[ plot_modality ][ plot_reader ] <= 20 ) {
		SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
		SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
		for( int i = 0 ; i < nlf_max[ plot_modality ][ plot_reader ] ; i++ ) {
			int xp = (int)( nlf_raw_froc[ plot_modality ][ plot_reader ][ i ] 
				/ nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left - 20 ) ) + rect.left + 10 ;
			int yp = -(int)( llf_raw_froc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top - 20 ) ) + rect.bottom - 10 ;
			dc.Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
		}
	} else {
		SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
		SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
		for( int i = 0 ; i < nlf_max[ plot_modality ][ plot_reader ] ; i++ ) {
			int xp = (int)( nlf_raw_froc[ plot_modality ][ plot_reader ][ i ] 
				/ nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left - 20 ) ) + rect.left + 10 ;
			int yp = -(int)( llf_raw_froc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top - 20 ) ) + rect.bottom - 10 ;
			dc.Arc( xp - 3, yp - 3, xp + 3, yp + 3, xp - 3, yp - 3, xp - 3, yp - 3 ) ;
		}
	}

	CStatic *status ;
	sprintf_s( str, "Reader: %s, Modality: %s", readerTable[ plot_reader ], modalityTable[ plot_modality ] ) ;
	status = (CStatic *)GetDlgItem( IDC_RAW_FROC_LABEL ) ;	status->SetWindowText( str ) ; ;

	*/

}

void CRawFROCPlotDialog::OnBnClickedRawFrocPrev()
{
	// TODO: Add your control notification handler code here
	Invalidate() ;
}

void CRawFROCPlotDialog::OnBnClickedRawFrocNext()
{
	// TODO: Add your control notification handler code here
	Invalidate() ;
}

void CRawFROCPlotDialog::OnBnClickedSaveCsv()
{
	// TODO: Add your control notification handler code here
	char filename[ 200 ], str[ 200 ] ;
	FILE *stream ;

	sprintf_s( filename, "%s_RAW_ROC.csv", selectedfilename ) ;

	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "FPF, TPF\n" ) ;
			for( int k = 0 ; k < fpf_max_roc[ i ][ j ] ; k++ ) {
				fprintf( stream, "%f, %f\n", fpf_raw_roc[ i ][ j ][ k ], tpf_raw_roc[ i ][ j ][ k ] ) ;
			}
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "Reader, AVERAGED, Treatment, %s\n", modalityTable[ i ] ) ;
		fprintf( stream, "FPF, TPF\n" ) ;
		for( int k = 0 ; k < N_AVG_RAW_PTS ; k++ ) {
			fprintf( stream, "%f, %f\n", fpf_avg_raw_roc[ k ], tpf_avg_raw_roc[ i ][ k ] ) ;
		}
	}

	fclose( stream ) ;
/*
	CStatic *status ;
	sprintf_s( str, "Saved CSV file: %s", filename ) ;
	status = (CStatic *)GetDlgItem( IDC_RAW_FROC_LABEL ) ;	status->SetWindowText( str ) ; ;
	*/

	sprintf_s( filename, "%s_RAW_AFROC.csv", selectedfilename ) ;

	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "FPF, LLF\n" ) ;
			for( int k = 0 ; k < fpf_max_afroc[ i ][ j ] ; k++ ) {
				fprintf( stream, "%f, %f\n", fpf_raw_afroc[ i ][ j ][ k ], llf_raw_afroc[ i ][ j ][ k ] ) ;
			}
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "Reader, AVERAGED, Treatment, %s\n", modalityTable[ i ] ) ;
		fprintf( stream, "FPF, LLF\n" ) ;
		for( int k = 0 ; k < N_AVG_RAW_PTS ; k++ ) {
			fprintf( stream, "%f, %f\n", fpf_avg_raw_afroc[ k ], llf_avg_raw_afroc[ i ][ k ] ) ;
		}
	}

	fclose( stream ) ;
	
	/*
	CStatic *status ;
	sprintf_s( str, "Saved CSV file: %s", filename ) ;
	status = (CStatic *)GetDlgItem( IDC_RAW_FROC_LABEL ) ;	status->SetWindowText( str ) ; ;
	*/

	sprintf_s( filename, "%s_RAW_FROC.csv", selectedfilename ) ;

	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "NLF, LLF\n" ) ;
			for( int k = 0 ; k < nlf_max_froc[ i ][ j ] ; k++ ) {
				fprintf( stream, "%f, %f\n", nlf_raw_froc[ i ][ j ][ k ], llf_raw_froc[ i ][ j ][ k ] ) ;
			}
		}
	}
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fprintf( stream, "Reader, AVERAGED, Treatment, %s\n", modalityTable[ i ] ) ;
		fprintf( stream, "NLF, LLF\n" ) ;
		for( int k = 0 ; k < N_AVG_RAW_PTS ; k++ ) {
			fprintf( stream, "%f, %f\n", nlf_avg_raw_froc[ i ][ k ], llf_avg_raw_froc[ i ][ k ] ) ;
		}
	}

	fclose( stream ) ;

	/*
	CStatic *status ;
	sprintf_s( str, "Saved CSV file: %s", filename ) ;
	status = (CStatic *)GetDlgItem( IDC_RAW_FROC_LABEL ) ;	status->SetWindowText( str ) ; ;
	*/
}



void CRawFROCPlotDialog::PlotIndividualCurve( CDC &dc, CRect rect, int mode )
{
	FillRect( dc.GetSafeHdc(), &rect, (HBRUSH)GetStockObject( WHITE_BRUSH ) ) ;
	dc.SetBkMode( TRANSPARENT ) ;

	{
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" ) ;
		lgFnt.lfHeight = 16 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		 
		// Set this font to DC     
		dc.SelectObject( &NewFont );
		if( mode == MODE_RAW_ROC )
			dc.TextOutA( ( rect.right - rect.left ) / 2 - 4 + rect.left, rect.bottom - 15, "FPF", 3 ) ;
		if( mode == MODE_RAW_AFROC )
			dc.TextOutA( ( rect.right - rect.left ) / 2 - 4 + rect.left, rect.bottom - 15, "FPF", 3 ) ;
		if( mode == MODE_RAW_FROC )
			dc.TextOutA( ( rect.right - rect.left ) / 2 - 4 + rect.left, rect.bottom - 15, "NLF", 3 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	{
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" );
		lgFnt.lfHeight = 16 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Give rotation as multiples of 10, so rotation will be 90 degrees
		lgFnt.lfOrientation = 900 ;
		lgFnt.lfEscapement =  900 ; 
		 
		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		//	const int DcRestorePoint2 = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		 
		// Set this font to DC     
		dc.SelectObject( &NewFont );     
		if( mode == MODE_RAW_ROC )
			dc.TextOutA( rect.left, ( rect.bottom - rect.top ) / 2 + 5 + rect.top, "TPF", 3 ) ;
		if( mode == MODE_RAW_AFROC )
			dc.TextOutA( rect.left, ( rect.bottom - rect.top ) / 2 + 5 + rect.top, "LLF", 3 ) ;
		if( mode == MODE_RAW_FROC )
			dc.TextOutA( rect.left, ( rect.bottom - rect.top ) / 2 + 5 + rect.top, "LLF", 3 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	rect.left += 30 ;
	rect.bottom -= 30 ;
	rect.right -= 10 ;
	rect.top += 10 ;

	char str[ 200 ] ;

	// axis
	SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( BLACK_PEN ) ) ;
	dc.MoveTo( rect.left, rect.bottom ) ; dc.LineTo( rect.right, rect.bottom ) ;
	dc.LineTo( rect.right, rect.top ) ; dc.LineTo( rect.left, rect.top ) ; dc.LineTo( rect.left, rect.bottom ) ;

	// graph name
	{
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" ) ;
		lgFnt.lfHeight = 20 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		 
		// Set this font to DC     
		dc.SelectObject( &NewFont );
		if( mode == MODE_RAW_ROC )
			dc.TextOutA( rect.right - 45, rect.bottom - 25, "ROC", 3 ) ;
		if( mode == MODE_RAW_AFROC )
			dc.TextOutA( rect.right - 65, rect.bottom - 25, "AFROC", 5 ) ;
		if( mode == MODE_RAW_FROC )
			dc.TextOutA( rect.right - 55, rect.bottom - 25, "FROC", 4 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	// ruler
	if( mode == MODE_RAW_ROC || mode == MODE_RAW_AFROC || mode == MODE_RAW_FROC ) {
		for( int i = 0 ; i < 4 ; i++ ) {
			dc.MoveTo( rect.left, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 5.0f * (float)( i + 1 ) ) ) ;
			dc.LineTo( rect.left + 6, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 5.0f * (float)( i + 1 ) ) ) ;
			dc.MoveTo( rect.right, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 5.0f * (float)( i + 1 ) ) ) ;
			dc.LineTo( rect.right - 6, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 5.0f * (float)( i + 1 ) ) ) ;
		}
		for( int i = 0 ; i < 19 ; i++ ) {
			dc.MoveTo( rect.left, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 20.0f * (float)( i + 1 ) ) ) ;
			dc.LineTo( rect.left + 3, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 20.0f * (float)( i + 1 ) ) ) ;
			dc.MoveTo( rect.right, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 20.0f * (float)( i + 1 ) ) ) ;
			dc.LineTo( rect.right - 3, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 20.0f * (float)( i + 1 ) ) ) ;
		}
			
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" ) ;
		lgFnt.lfHeight = 12 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		dc.SelectObject( &NewFont );     
		 
		for( int i = 0 ; i < 5 ; i++ ) {
			sprintf( str, "%3.1f", (float)( i + 1 ) * 0.2f ) ;
			dc.TextOutA( rect.left - 15, rect.bottom - (int)( (float)( rect.bottom - rect.top ) / 5.0f * (float)( i + 1 ) ) - 5, str, strlen( str ) ) ;
		}

		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	if( mode == MODE_RAW_ROC || mode == MODE_RAW_AFROC ) {
		for( int i = 0 ; i < 4 ; i++ ) {
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 5.0f * (float)( i + 1 ) ), rect.bottom ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 5.0f * (float)( i + 1 ) ), rect.bottom - 6 ) ;
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 5.0f * (float)( i + 1 ) ), rect.top ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 5.0f * (float)( i + 1 ) ), rect.top + 6 ) ;
		}
		for( int i = 0 ; i < 19 ; i++ ) {
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 20.0f * (float)( i + 1 ) ), rect.bottom ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 20.0f * (float)( i + 1 ) ), rect.bottom - 3 ) ;
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 20.0f * (float)( i + 1 ) ), rect.top ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 20.0f * (float)( i + 1 ) ), rect.top + 3 ) ;
		}
			
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" ) ;
		lgFnt.lfHeight = 12 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		dc.SelectObject( &NewFont );     
		 
		for( int i = 0 ; i < 5 ; i++ ) {
			sprintf( str, "%3.1f", (float)( i + 1 ) * 0.2f ) ;
			dc.TextOutA( rect.left + (int)( (float)( rect.right - rect.left ) / 5.0f * (float)( i + 1 ) ) - 8, rect.bottom + 4, str, strlen( str ) ) ;
		}

		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}
	float max_x_FROC = -1e15 ;
	if( mode == MODE_RAW_FROC ) {

		if( plot_reader < 0 ) {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					if( plot_modality_arr[ i ] != 0 )
						if( nlf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] - 1 ] > max_x_FROC ) max_x_FROC = nlf_raw_froc[ i ][ j ][ nlf_max_froc[ i ][ j ] - 1 ] ;
				}
			}
		} else {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				if( plot_modality_arr[ i ] != 0 ) {
					if( nlf_raw_froc[ i ][ plot_reader ][ nlf_max_froc[ i ][ plot_reader ] - 1 ] > max_x_FROC ) 
						max_x_FROC = nlf_raw_froc[ i ][ plot_reader ][ nlf_max_froc[ i ][ plot_reader ] - 1 ] ;
				}
			}
		}
		if( max_x_FROC < 0 ) max_x_FROC = 1.0f ;
		/*
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( nlf_max[ i ][ j ] > max_x_FROC ) max_x_FROC = nlf_max[ i ][ j ] ;
			}
		}
		*/
		

		if( max_x_FROC * 10.0f > (int)( max_x_FROC * 10.0f ) )
			max_x_FROC = (float)( (int)( max_x_FROC * 10.0f ) + 1 ) / 10.0f ;
		else 
			max_x_FROC = (float)( (int)( max_x_FROC * 10.0f ) ) / 10.0f ;

		for( int i = 0 ; i < 3 ; i++ ) {
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 4.0f * (float)( i + 1 ) ), rect.bottom ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 4.0f * (float)( i + 1 ) ), rect.bottom - 6 ) ;
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 4.0f * (float)( i + 1 ) ), rect.top ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 4.0f * (float)( i + 1 ) ), rect.top + 6 ) ;
		}
		for( int i = 0 ; i < 15 ; i++ ) {
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 16.0f * (float)( i + 1 ) ), rect.bottom ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 16.0f * (float)( i + 1 ) ), rect.bottom - 3 ) ;
			dc.MoveTo( rect.left + (int)( (float)( rect.right - rect.left ) / 16.0f * (float)( i + 1 ) ), rect.top ) ;
			dc.LineTo( rect.left + (int)( (float)( rect.right - rect.left ) / 16.0f * (float)( i + 1 ) ), rect.top + 3 ) ;
		}
			
		CFont NewFont;     
		LOGFONT lgFnt = { 0 };     
		GetFont()->GetLogFont(&lgFnt);  

		// Modify this existing font //
		 
		// Face name is important, some font's like MS Sans Serif doesn't work
		strcpy_s( lgFnt.lfFaceName, "Arial" ) ;
		lgFnt.lfHeight = 12 ;
		lgFnt.lfWeight = FW_BOLD ;

		// Create a font from this log font     
		VERIFY( NewFont.CreateFontIndirect( &lgFnt )); 
		 
		// Save this DC    
		const int DcRestorePoint = dc.SaveDC();      
		dc.SetBkMode( TRANSPARENT );  
		dc.SelectObject( &NewFont );     
		 
		for( int i = 0 ; i < 4 ; i++ ) {
			sprintf( str, "%4.2f", (float)( i + 1 ) * 0.25f * max_x_FROC ) ;
			dc.TextOutA( rect.left + (int)( (float)( rect.right - rect.left ) / 4.0f * (float)( i + 1 ) ) - 12, rect.bottom + 4, str, strlen( str ) ) ;
		}

		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );

	}


	/*
	if( mode == MODE_RAW_ROC )
		PlotROC( this, &dc,  rect.left, rect.top, rect.right, rect.bottom ) ;
	if( mode == MODE_RAW_AFROC )
		PlotAFROC( this, &dc,  rect.left, rect.top, rect.right, rect.bottom ) ;
	if( mode == MODE_RAW_FROC )
		PlotFROC( this, &dc,  rect.left, rect.top, rect.right, rect.bottom ) ;
	if( mode == PLOT_PDFS )
		PlotPDFS( this, &dc,  rect.left, rect.top, rect.right, rect.bottom ) ;
	*/
	if( mode == MODE_RAW_ROC ) {
		for( int plot_modality = 0 ; plot_modality < N_TREATMENTS ; plot_modality++ ) {
			if( plot_modality_arr[ plot_modality ] == 0 ) continue ;
			SetDCPenColor( dc.GetSafeHdc(), RGB( plot_color_table[ plot_modality % 12 ][ 0 ], plot_color_table[ plot_modality % 12 ][ 1 ], plot_color_table[ plot_modality % 12 ][ 2 ] ) ) ;
			SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
			if( plot_reader < 0 ) {
				dc.MoveTo( rect.left, rect.bottom ) ;
				for( int i = 0 ; i < N_AVG_RAW_PTS ; i++ ) {
					dc.LineTo( 
						(int)( fpf_avg_raw_roc[ i ] * ( rect.right - rect.left ) ) + rect.left,
						-(int)( tpf_avg_raw_roc[ plot_modality ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ) ;
				}
			} else {
				dc.MoveTo( rect.left, rect.bottom ) ;
				for( int i = 0 ; i < fpf_max_roc[ plot_modality ][ plot_reader ] ; i++ ) {
					dc.LineTo( 
						(int)( fpf_raw_roc[ plot_modality ][ plot_reader ][ i ] 
					/ fpf_raw_roc[ plot_modality ][ plot_reader ][ fpf_max_roc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left,
						-(int)( tpf_raw_roc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ) ;
				}
				if( fpf_max_roc[ plot_modality ][ plot_reader ] <= 20 ) {
//					SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
//					SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
					for( int i = 0 ; i < fpf_max_roc[ plot_modality ][ plot_reader ] - 1 ; i++ ) {
						int xp = (int)( fpf_raw_roc[ plot_modality ][ plot_reader ][ i ] 
							/ fpf_raw_roc[ plot_modality ][ plot_reader ][ fpf_max_roc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left ;
						int yp = -(int)( tpf_raw_roc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ;
						dc.Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
					}
				} else {
//					SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
//					SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
					for( int i = 0 ; i < fpf_max_roc[ plot_modality ][ plot_reader ] - 1 ; i++ ) {
						int xp = (int)( fpf_raw_roc[ plot_modality ][ plot_reader ][ i ] 
							/ fpf_raw_roc[ plot_modality ][ plot_reader ][ fpf_max_roc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left ;
						int yp = -(int)( tpf_raw_roc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ;
						dc.Arc( xp - 3, yp - 3, xp + 3, yp + 3, xp - 3, yp - 3, xp - 3, yp - 3 ) ;
					}
				}
			}
		}
	}
	if( mode == MODE_RAW_AFROC ) {
		for( int plot_modality = 0 ; plot_modality < N_TREATMENTS ; plot_modality++ ) {
			if( plot_modality_arr[ plot_modality ] == 0 ) continue ;
			SetDCPenColor( dc.GetSafeHdc(), RGB( plot_color_table[ plot_modality % 12 ][ 0 ], plot_color_table[ plot_modality % 12 ][ 1 ], plot_color_table[ plot_modality % 12 ][ 2 ] ) ) ;
			SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
			if( plot_reader < 0 ) {
				dc.MoveTo( rect.left, rect.bottom ) ;
				for( int i = 0 ; i < N_AVG_RAW_PTS ; i++ ) {
					dc.LineTo( 
						(int)( fpf_avg_raw_afroc[ i ] * ( rect.right - rect.left ) ) + rect.left,
						-(int)( llf_avg_raw_afroc[ plot_modality ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ) ;
				}
			} else {
				dc.MoveTo( rect.left, rect.bottom ) ;
				for( int i = 0 ; i < fpf_max_afroc[ plot_modality ][ plot_reader ] ; i++ ) {
					dc.LineTo( 
						(int)( fpf_raw_afroc[ plot_modality ][ plot_reader ][ i ] 
					/ fpf_raw_afroc[ plot_modality ][ plot_reader ][ fpf_max_afroc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left,
						-(int)( llf_raw_afroc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ) ;
				}
				if( fpf_max_afroc[ plot_modality ][ plot_reader ] <= 20 ) {
//					SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
//					SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
					for( int i = 0 ; i < fpf_max_afroc[ plot_modality ][ plot_reader ] - 1 ; i++ ) {
						int xp = (int)( fpf_raw_afroc[ plot_modality ][ plot_reader ][ i ] 
							/ fpf_raw_afroc[ plot_modality ][ plot_reader ][ fpf_max_afroc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left ;
						int yp = -(int)( llf_raw_afroc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ;
						dc.Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
					}
				} else {
//					SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
//					SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
					for( int i = 0 ; i < fpf_max_afroc[ plot_modality ][ plot_reader ] - 1 ; i++ ) {
						int xp = (int)( fpf_raw_afroc[ plot_modality ][ plot_reader ][ i ] 
							/ fpf_raw_afroc[ plot_modality ][ plot_reader ][ fpf_max_afroc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left ;
						int yp = -(int)( llf_raw_afroc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ;
						dc.Arc( xp - 3, yp - 3, xp + 3, yp + 3, xp - 3, yp - 3, xp - 3, yp - 3 ) ;
					}
				}
			}
		}
	}
	if( mode == MODE_RAW_FROC ) {
		for( int plot_modality = 0 ; plot_modality < N_TREATMENTS ; plot_modality++ ) {
			if( plot_modality_arr[ plot_modality ] == 0 ) continue ;
			SetDCPenColor( dc.GetSafeHdc(), RGB( plot_color_table[ plot_modality % 12 ][ 0 ], plot_color_table[ plot_modality % 12 ][ 1 ], plot_color_table[ plot_modality % 12 ][ 2 ] ) ) ;
			SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
			if( plot_reader < 0 ) {
				dc.MoveTo( rect.left, rect.bottom ) ;
				for( int i = 0 ; i < N_AVG_RAW_PTS ; i++ ) {
					dc.LineTo( 
//						(int)( nlf_avg_raw_froc[ i ] / nlf_avg_raw_froc[ N_AVG_RAW_PTS - 1 ] * ( rect.right - rect.left ) ) + rect.left,
						(int)( nlf_avg_raw_froc[ plot_modality ][ i ] / max_x_FROC * ( rect.right - rect.left ) ) + rect.left,
						-(int)( llf_avg_raw_froc[ plot_modality ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ) ;
				}
			} else {
				dc.MoveTo( rect.left, rect.bottom ) ;
				for( int i = 0 ; i < nlf_max_froc[ plot_modality ][ plot_reader ] ; i++ ) {
					dc.LineTo( 
						(int)( nlf_raw_froc[ plot_modality ][ plot_reader ][ i ] 
//					/ nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max_froc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left,
					/ max_x_FROC * ( rect.right - rect.left ) ) + rect.left,
						-(int)( llf_raw_froc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ) ;
				}
				if( nlf_max_froc[ plot_modality ][ plot_reader ] <= 20 ) {
//					SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
//					SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
					for( int i = 0 ; i < nlf_max_froc[ plot_modality ][ plot_reader ] ; i++ ) {
						int xp = (int)( nlf_raw_froc[ plot_modality ][ plot_reader ][ i ] 
//							/ nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max_froc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left ;
							/ max_x_FROC * ( rect.right - rect.left ) ) + rect.left ;
						int yp = -(int)( llf_raw_froc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ;
						dc.Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
					}
				} else {
//					SetDCPenColor( dc.GetSafeHdc(), RGB( 64, 192, 64 ) ) ;
//					SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
					for( int i = 0 ; i < nlf_max_froc[ plot_modality ][ plot_reader ] ; i++ ) {
						int xp = (int)( nlf_raw_froc[ plot_modality ][ plot_reader ][ i ] 
//							/ nlf_raw_froc[ plot_modality ][ plot_reader ][ nlf_max_froc[ plot_modality ][ plot_reader ] - 1 ] * ( rect.right - rect.left ) ) + rect.left ;
							/ max_x_FROC * ( rect.right - rect.left ) ) + rect.left ;
						int yp = -(int)( llf_raw_froc[ plot_modality ][ plot_reader ][ i ] * ( rect.bottom - rect.top ) ) + rect.bottom ;
						dc.Arc( xp - 3, yp - 3, xp + 3, yp + 3, xp - 3, yp - 3, xp - 3, yp - 3 ) ;
					}
				}
			}
		}
	}
}

void CRawFROCPlotDialog::OnLbnSelchangeRawPlotReader()
{
	// TODO: Add your control notification handler code here
	plot_reader = m_list_reader.GetCurSel() ;
	if( MAX_READERS > 1 ) {
		if( plot_reader == 0 ) {
			// averaged plot
			plot_reader-- ;
			Invalidate() ;
		} else {
			plot_reader-- ;
			Invalidate() ;
		}
	} else {
		Invalidate() ;
	}
}


void CRawFROCPlotDialog::OnLbnSelchangeRawPlotModality()
{
	// TODO: Add your control notification handler code here
//	plot_modality = m_list_modality.GetCurSel() ;
	int nCount = m_list_modality.GetSelCount() ;
	CArray<int,int> aryListBoxSel ;

	aryListBoxSel.SetSize( nCount ) ;
	m_list_modality.GetSelItems( nCount, aryListBoxSel.GetData() ) ; 
	
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		plot_modality_arr[ i ] = 0 ;
	}
	for( int i = 0 ; i < nCount ; i++ ) {
		plot_modality_arr[ aryListBoxSel.GetAt( i ) ] = 1 ;
	}

	Invalidate() ;
}
