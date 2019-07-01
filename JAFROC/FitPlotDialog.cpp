// FitPlotDialog.cpp : implementation file
//

#include "stdafx.h"
#include "JAFROC.h"
#include "FitPlotDialog.h"
#include <math.h>
#include "common.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define MODE_FIT_ROC	20
#define MODE_FIT_AFROC	22
#define MODE_FIT_FROC	24



#define N_TAPS	100



extern CString selectedfilename, datafilename, outputfilename ;
int fit_plot_first ;
int fit_plot_reader ;
int fit_plot_modality ;
int *fit_plot_modality_arr = NULL ;



// hjy sm fit plotting routines here
void PlotROCPoints( float *fpf_array, float *tpf_array )
{
	float mu_roc, lambda_roc, nu_roc ;
	if( fit_plot_reader < 0 ) {
		mu_roc = roc_fit_avg_parms[ fit_plot_modality ][ 2 ] ;
		lambda_roc = roc_fit_avg_parms[ fit_plot_modality ][ 0 ] ;
		nu_roc = roc_fit_avg_parms[ fit_plot_modality ][ 3 ] ;
	} else {
		mu_roc = roc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 2 ] ;
		lambda_roc = roc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 0 ] ;
		nu_roc = roc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 3 ] ;
	}

	float delta = 1e15f ;
	float midzeta = -2.0f ;
	float deltazeta = 0.5f ;

	while( fabs( delta ) > 1e-2 ) {
		midzeta = midzeta + deltazeta ;
		float xx = fpf_poi( midzeta, lambda_roc ) ;
		float yy = tpf_poi( midzeta, mu_roc, lambda_roc, nu_roc ) ;
		delta = sqrt( xx * xx + yy * yy ) - 
			sqrt( pow( xx - fpf_poi( -20.0f, lambda_roc ), 2.0f ) + pow( yy - tpf_poi( -20.0f, mu_roc, lambda_roc, nu_roc ), 2.0f ) ) ;
		if( delta > 0.0f && deltazeta < 0.0f ) deltazeta = -deltazeta / 2.0f ;
		if( delta <= 0.0f && deltazeta >= 0.0f ) deltazeta = -deltazeta / 2.0f ;
	}
	float mid_x = fpf_poi( midzeta, lambda_roc ) ;
	float mid_y = tpf_poi( midzeta, mu_roc, lambda_roc, nu_roc ) ;
	float taps_y_roc[ N_TAPS + 1 ], taps_x_roc[ N_TAPS + 1 ] ;
	for( int i = 0 ; i < N_TAPS ; i++ ) {
		taps_y_roc[ i ] = (float)i * mid_y / (float)N_TAPS ;
	}
	for( int i = 0 ; i <= N_TAPS ; i++ ) {
		taps_x_roc[ i ] = (float)i * ( fpf_poi( -20.0f, lambda_roc ) - mid_x ) / (float)N_TAPS + mid_x ;
	}

	for( int k = 0 ; k < N_TAPS ; k++ ) {
		float zeta_roc ;
		if( k == 0 ) 
			zeta_roc = 20.0f ;
		else 
			zeta_roc = inv_tpf_poi( taps_y_roc[ k ], mu_roc, lambda_roc, nu_roc ) ;
		float x1 = fpf_poi( zeta_roc, lambda_roc ) ;
		float y1 = tpf_poi( zeta_roc, mu_roc, lambda_roc, nu_roc ) ;
		fpf_array[ k ] = x1 ;
		tpf_array[ k ] = y1 ;
	}
	for( int k = 0 ; k <= N_TAPS ; k++ ) {
		float zeta_roc ;
		if( k == N_TAPS ) 
			zeta_roc = -20.0f ;
		else
			zeta_roc = inv_fpf_poi( taps_x_roc[ k ], lambda_roc ) ;
		float x1 = fpf_poi( zeta_roc, lambda_roc ) ;
		float y1 = tpf_poi( zeta_roc, mu_roc, lambda_roc, nu_roc ) ;
		fpf_array[ N_TAPS + k ] = x1 ;
		tpf_array[ N_TAPS + k ] = y1 ;
	}
}

void PlotROC( CDC *dc, int left, int top, int right, int bottom )
{
	float fpf[ N_TAPS * 2 + 1 ], tpf[ N_TAPS * 2 + 1 ] ;

	PlotROCPoints( fpf, tpf ) ;

	dc->MoveTo( left, bottom ) ;
	for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
		float x1 = fpf[ k ] ;
		float y1 = tpf[ k ] ;
		int xp = left + (int)( x1 * (float)( right - left ) ) ;
		int yp = bottom - (int)( y1 * (float)( bottom - top ) ) ;
		dc->LineTo( xp, yp ) ;
	}
	HPEN pen, oldpen ;
	pen = CreatePen( PS_DOT, 1, RGB( plot_color_table[ fit_plot_modality % 12 ][ 0 ], plot_color_table[ fit_plot_modality % 12 ][ 1 ], plot_color_table[ fit_plot_modality % 12 ][ 2 ] ) ) ;
	oldpen = (HPEN)SelectObject( dc->GetSafeHdc(), pen ) ;
	dc->LineTo( right, top ) ;
	SelectObject( dc->GetSafeHdc(), oldpen ) ;

	if( fit_plot_reader < 0 ) return ;
	// op points
//	SetDCPenColor( dc->GetSafeHdc(), RGB( 192, 0, 0 ) ) ;
//	SelectObject( dc->GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
	for( int k = 0 ; k < number_of_bins_ROC[ fit_plot_modality ][ fit_plot_reader ] ; k++ ) {
		int xp = left + (int)( FPF_BIN_ROC[ fit_plot_modality ][ fit_plot_reader ][ k ] * (float)( right - left ) ) ;
		int yp = bottom - (int)( TPF_BIN_ROC[ fit_plot_modality ][ fit_plot_reader ][ k ] * (float)( bottom - top ) ) ;
		dc->Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
	}
}

void PlotAFROCPoints( float *fpf_array, float *llf_array )
{
	float mu_afroc, lambda_afroc, nu_afroc ;
	if( fit_plot_reader < 0 ) {
		mu_afroc = afroc_fit_avg_parms[ fit_plot_modality ][ 2 ] ;
		lambda_afroc = afroc_fit_avg_parms[ fit_plot_modality ][ 0 ] ;
		nu_afroc = afroc_fit_avg_parms[ fit_plot_modality ][ 3 ] ;
	} else {
		mu_afroc = afroc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 2 ] ;
		lambda_afroc = afroc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 0 ] ;
		nu_afroc = afroc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 3 ] ;
	}

	float delta = 1e15f ;
	float midzeta = -2.0f ;
	float deltazeta = 0.5f ;

	while( fabs( delta ) > 1e-2 ) {
		midzeta = midzeta + deltazeta ;
		float xx = fpf_poi( midzeta, lambda_afroc ) ;
		float yy = llf_poi( midzeta, mu_afroc, nu_afroc ) ;
		delta = sqrt( xx * xx + yy * yy ) - 
			sqrt( pow( xx - fpf_poi( -20.0f, lambda_afroc ), 2.0f ) + pow( yy - llf_poi( -20.0f, mu_afroc, nu_afroc ), 2.0f ) ) ;
		if( delta > 0.0f && deltazeta < 0.0f ) deltazeta = -deltazeta / 2.0f ;
		if( delta <= 0.0f && deltazeta >= 0.0f ) deltazeta = -deltazeta / 2.0f ;
	}
	float mid_x = fpf_poi( midzeta, lambda_afroc ) ;
	float mid_y = llf_poi( midzeta, mu_afroc, nu_afroc ) ;
	float taps_y_afroc[ N_TAPS + 1 ], taps_x_afroc[ N_TAPS + 1 ] ;
	for( int i = 0 ; i < N_TAPS ; i++ ) {
		taps_y_afroc[ i ] = (float)i * mid_y / (float)N_TAPS ;
	}
	for( int i = 0 ; i <= N_TAPS ; i++ ) {
		taps_x_afroc[ i ] = (float)i * ( fpf_poi( -20.0f, lambda_afroc ) - mid_x ) / (float)N_TAPS + mid_x ;
	}

	for( int k = 0 ; k < N_TAPS ; k++ ) {
		float zeta_afroc ;
		if( k == 0 ) 
			zeta_afroc = 20.0f ;
		else 
			zeta_afroc = inv_llf_poi( taps_y_afroc[ k ], mu_afroc, nu_afroc ) ;
		float x1 = fpf_poi( zeta_afroc, lambda_afroc ) ;
		float y1 = llf_poi( zeta_afroc, mu_afroc, nu_afroc ) ;
		fpf_array[ k ] = x1 ;
		llf_array[ k ] = y1 ;
	}
	for( int k = 0 ; k <= N_TAPS ; k++ ) {
		float zeta_afroc ;
		if( k == N_TAPS ) 
			zeta_afroc = -20.0f ;
		else
			zeta_afroc = inv_fpf_poi( taps_x_afroc[ k ], lambda_afroc ) ;
		float x1 = fpf_poi( zeta_afroc, lambda_afroc ) ;
		float y1 = llf_poi( zeta_afroc, mu_afroc, nu_afroc ) ;
		fpf_array[ k + N_TAPS ] = x1 ;
		llf_array[ k + N_TAPS ] = y1 ;
	}
}

void PlotAFROC( CDC *dc, int left, int top, int right, int bottom )
{
	float fpf[ N_TAPS * 2 + 1 ], llf[ N_TAPS * 2 + 1 ] ;

	PlotAFROCPoints( fpf, llf ) ;

	dc->MoveTo( left, bottom ) ;
	for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
		float x1 = fpf[ k ] ;
		float y1 = llf[ k ] ;
		int xp = left + (int)( x1 * (float)( right - left ) ) ;
		int yp = bottom - (int)( y1 * (float)( bottom - top ) ) ;
		dc->LineTo( xp, yp ) ;
	}
	HPEN pen, oldpen ;
	pen = CreatePen( PS_DOT, 1, RGB( plot_color_table[ fit_plot_modality % 12 ][ 0 ], plot_color_table[ fit_plot_modality % 12 ][ 1 ], plot_color_table[ fit_plot_modality % 12 ][ 2 ] ) ) ;
	oldpen = (HPEN)SelectObject( dc->GetSafeHdc(), pen ) ;
	dc->LineTo( right, top ) ;
	SelectObject( dc->GetSafeHdc(), oldpen ) ;

	if( fit_plot_reader < 0 ) return ;
	// op points
	//	NLF_BIN_FROC[ i ][ j ][ k ], LLF_BIN_FROC[ i ][ j ][ k ]
//	SetDCPenColor( dc->GetSafeHdc(), RGB( 192, 0, 0 ) ) ;
//	SelectObject( dc->GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
	for( int k = 0 ; k < number_of_bins_AFROC[ fit_plot_modality ][ fit_plot_reader ] ; k++ ) {
		int xp = left + (int)( FPF_BIN_AFROC[ fit_plot_modality ][ fit_plot_reader ][ k ] * (float)( right - left ) ) ;
		int yp = bottom - (int)( LLF_BIN_AFROC[ fit_plot_modality ][ fit_plot_reader ][ k ] * (float)( bottom - top ) ) ;
		dc->Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
	}
}

void PlotFROCPoints( float *nlf_array, float *llf_array )
{
	float max_x_FROC = UNINITIALIZED ;
	if( fit_plot_reader < 0 ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( fit_plot_modality_arr[ i ] == 0 ) continue ;
			if( froc_fit_avg_parms[ i ][ 0 ] > max_x_FROC ) max_x_FROC = froc_fit_avg_parms[ i ][ 0 ] ;
		}
	} else {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( fit_plot_modality_arr[ i ] == 0 ) continue ;
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( fit_plot_reader != j ) continue ;
				if( froc_fit_parms[ i ][ j ][ 0 ] > max_x_FROC ) max_x_FROC = froc_fit_parms[ i ][ j ][ 0 ] ;
			}
		}
	}
//	max_x_FROC = (float)( (int)( max_x_FROC * 100.0f ) ) / 100.0f + 0.2f ;
	if( max_x_FROC * 10.0f > (int)( max_x_FROC * 10.0f ) )
		max_x_FROC = (float)( (int)( max_x_FROC * 10.0f ) + 1 ) / 10.0f ;
	else 
		max_x_FROC = (float)( (int)( max_x_FROC * 10.0f ) ) / 10.0f ;

	float mu_froc, lambda_froc, nu_froc ;
	if( fit_plot_reader < 0 ) {
		mu_froc = froc_fit_avg_parms[ fit_plot_modality ][ 2 ] ;
		lambda_froc = froc_fit_avg_parms[ fit_plot_modality ][ 0 ] ;
		nu_froc = froc_fit_avg_parms[ fit_plot_modality ][ 3 ] ;
	} else {
		mu_froc = froc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 2 ] ;
		lambda_froc = froc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 0 ] ;
		nu_froc = froc_fit_parms[ fit_plot_modality ][ fit_plot_reader ][ 3 ] ;
	}

	float delta = 1e15f ;
	float midzeta = -2.0f ;
	float deltazeta = 0.5f ;

	float zeta_min ;
	if( max_x_FROC > lambda_froc ) zeta_min = -20.0f ;
	else zeta_min = inv_nlf_poi( max_x_FROC, lambda_froc ) ;

	while( fabs( delta ) > 1e-2 ) {
		midzeta = midzeta + deltazeta ;
		float xx = nlf_poi( midzeta, lambda_froc ) ;
		float yy = llf_poi( midzeta, mu_froc, nu_froc ) ;
		if( lambda_froc > max_x_FROC ) 
			delta = sqrt( xx * xx + yy * yy ) - 
				sqrt( pow( xx - max_x_FROC, 2.0f ) + pow( yy - llf_poi( -20.0f, mu_froc, nu_froc ), 2.0f ) ) ;
		else
			delta = sqrt( xx * xx + yy * yy ) - 
				sqrt( pow( xx - lambda_froc, 2.0f ) + pow( yy - llf_poi( -20.0f, mu_froc, nu_froc ), 2.0f ) ) ;
		if( delta > 0.0f && deltazeta < 0.0f ) deltazeta = -deltazeta / 2.0f ;
		if( delta <= 0.0f && deltazeta >= 0.0f ) deltazeta = -deltazeta / 2.0f ;
	}
	float mid_x = nlf_poi( midzeta, lambda_froc ) ;
	float mid_y = llf_poi( inv_nlf_poi( mid_x, lambda_froc ), mu_froc, nu_froc ) ;
	float taps_y_froc[ N_TAPS + 1 ], taps_x_froc[ N_TAPS + 1 ] ;
	for( int i = 0 ; i < N_TAPS ; i++ ) {
		taps_y_froc[ i ] = (float)i * mid_y / (float)N_TAPS ;
	}
	for( int i = 0 ; i <= N_TAPS ; i++ ) {
		taps_x_froc[ i ] = (float)i * ( max_x_FROC - mid_x ) / (float)N_TAPS + mid_x ;
	}

	for( int k = 0 ; k < N_TAPS ; k++ ) {
		float zeta_froc ;
		if( k == 0 ) 
			zeta_froc = 20.0f ;
		else 
			zeta_froc = inv_llf_poi( taps_y_froc[ k ], mu_froc, nu_froc ) ;
		float x1 = nlf_poi( zeta_froc, lambda_froc ) ;
		float y1 = llf_poi( zeta_froc, mu_froc, nu_froc ) ;
		nlf_array[ k ] = x1 ;
		llf_array[ k ] = y1 ;
	}
	for( int k = 0 ; k <= N_TAPS ; k++ ) {
		float zeta_froc ;
		if( k == N_TAPS ) {
			if( taps_x_froc[ N_TAPS ] >= lambda_froc ) zeta_froc = -20.0f ;
			else zeta_froc = inv_nlf_poi( taps_x_froc[ N_TAPS ], lambda_froc ) ;
		} else {
			if( taps_x_froc[ k ] >= lambda_froc ) zeta_froc = -20.0f ;
			else zeta_froc = inv_nlf_poi( taps_x_froc[ k ], lambda_froc ) ;
		}
		float x1 = nlf_poi( zeta_froc, lambda_froc ) ;
		float y1 = llf_poi( zeta_froc, mu_froc, nu_froc ) ;
		nlf_array[ k + N_TAPS ] = x1 ;
		llf_array[ k + N_TAPS ] = y1 ;
	}
}

void PlotFROC( CDC *dc, int left, int top, int right, int bottom )
{
	float nlf[ N_TAPS * 2 + 1 ], llf[ N_TAPS * 2 + 1 ] ;

	PlotFROCPoints( nlf, llf ) ;

	float max_x_FROC = UNINITIALIZED ;
	if( fit_plot_reader < 0 ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( fit_plot_modality_arr[ i ] == 0 ) continue ;
			if( froc_fit_avg_parms[ i ][ 0 ] > max_x_FROC ) max_x_FROC = froc_fit_avg_parms[ i ][ 0 ] ;
		}
	} else {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( fit_plot_modality_arr[ i ] == 0 ) continue ;
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				if( fit_plot_reader != j ) continue ;
				if( froc_fit_parms[ i ][ j ][ 0 ] > max_x_FROC ) max_x_FROC = froc_fit_parms[ i ][ j ][ 0 ] ;
			}
		}
	}
//	max_x_FROC = (float)( (int)( max_x_FROC * 100.0f ) ) / 100.0f + 0.2f ;
	if( max_x_FROC * 10.0f > (int)( max_x_FROC * 10.0f ) )
		max_x_FROC = (float)( (int)( max_x_FROC * 10.0f ) + 1 ) / 10.0f ;
	else 
		max_x_FROC = (float)( (int)( max_x_FROC * 10.0f ) ) / 10.0f ;

	dc->MoveTo( left, bottom ) ;
	for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
		float x1 = nlf[ k ] ;
		float y1 = llf[ k ] ;
		int xp = left + (int)( x1 * (float)( right - left ) / max_x_FROC ) ;
		int yp = bottom - (int)( y1 * (float)( bottom - top ) ) ;
		dc->LineTo( xp, yp ) ;
	}
	if( fit_plot_reader < 0 ) return ;
	// op points
	//	NLF_BIN_FROC[ i ][ j ][ k ], LLF_BIN_FROC[ i ][ j ][ k ]
//	SetDCPenColor( dc->GetSafeHdc(), RGB( 192, 0, 0 ) ) ;
//	SelectObject( dc->GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
	for( int k = 0 ; k < number_of_bins_FROC[ fit_plot_modality ][ fit_plot_reader ] ; k++ ) {
		int xp = left + (int)( NLF_BIN_FROC[ fit_plot_modality ][ fit_plot_reader ][ k ] * (float)( right - left ) / max_x_FROC ) ;
		int yp = bottom - (int)( LLF_BIN_FROC[ fit_plot_modality ][ fit_plot_reader ][ k ] * (float)( bottom - top ) ) ;
		dc->Arc( xp - 5, yp - 5, xp + 5, yp + 5, xp - 5, yp - 5, xp - 5, yp - 5 ) ;
	}
}


// CFitPlotDialog dialog

IMPLEMENT_DYNAMIC(CFitPlotDialog, CDialog)

CFitPlotDialog::CFitPlotDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFitPlotDialog::IDD, pParent)
{
	fit_plot_reader = 0 ;

	fit_plot_first = 1 ;

	if( fit_plot_modality_arr != NULL ) delete[] fit_plot_modality_arr ;
	fit_plot_modality_arr = new int[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fit_plot_modality_arr[ i ] = 0 ;
	}
	fit_plot_modality_arr[ 0 ] = 1 ;

}

CFitPlotDialog::~CFitPlotDialog()
{
	if( fit_plot_modality_arr != NULL ) {
		delete[] fit_plot_modality_arr ;
		fit_plot_modality_arr = NULL ;
	}
}

void CFitPlotDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLOT_READER, m_list_reader);
	DDX_Control(pDX, IDC_PLOT_MODALITY, m_list_modality);
	DDX_Control(pDX, IDC_PARAM_SUMMARY, m_param_summary);
}


BEGIN_MESSAGE_MAP(CFitPlotDialog, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FIT_PLOT_PREV, &CFitPlotDialog::OnBnClickedFitPlotPrev)
	ON_BN_CLICKED(IDC_FIT_PLOT_NEXT, &CFitPlotDialog::OnBnClickedFitPlotNext)
	ON_BN_CLICKED(IDC_SM_FIT_SAVE_CSV, &CFitPlotDialog::OnBnClickedSmFitSaveCsv)
	ON_LBN_SELCHANGE(IDC_PLOT_READER, &CFitPlotDialog::OnLbnSelchangePlotReader)
	ON_LBN_SELCHANGE(IDC_PLOT_MODALITY, &CFitPlotDialog::OnLbnSelchangePlotModality)
END_MESSAGE_MAP()


// CFitPlotDialog message handlers

void CFitPlotDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	RECT rect ;
	GetClientRect( &rect ) ;
	rect.left = rect.right / 2 ;
	rect.left += 5 ;
	rect.right -= 10 ;
	rect.top += 10 ;
	rect.bottom /= 2 ;
	rect.bottom -= 5 ;

	PlotIndividualCurve( dc, rect, MODE_FIT_ROC ) ;

	GetClientRect( &rect ) ;
	rect.left = rect.right / 2 ;
	rect.left += 5 ;
	rect.right -= 10 ;
	rect.top = rect.bottom / 2 ;
	rect.top += 5 ;
	rect.bottom -= 10 ;

	PlotIndividualCurve( dc, rect, MODE_FIT_AFROC ) ;

	GetClientRect( &rect ) ;
	rect.left += 10 ;
	rect.right /= 2 ;
	rect.right -= 5 ;
	rect.top = rect.bottom / 2 ;
	rect.top += 5 ;
	rect.bottom -= 10 ;

	PlotIndividualCurve( dc, rect, MODE_FIT_FROC ) ;

	if( fit_plot_first ) {
		fit_plot_first = 0 ;

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

		WriteSummary() ;
	}

/*


	if( this->sm_fit_plot_method == FIT_CURVE_ROC )
		this->SetWindowTextA( "Search Model ROC Fit Curve" ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_AFROC )
		this->SetWindowTextA( "Search Model AFROC Fit Curve" ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_FROC )
		this->SetWindowTextA( "Search Model FROC Fit Curve" ) ;

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
		if( this->sm_fit_plot_method == FIT_CURVE_ROC )
			dc.TextOutA( rect.left + 300, rect.bottom - 15, "FPF", 3 ) ;
		if( this->sm_fit_plot_method == FIT_CURVE_AFROC )
			dc.TextOutA( rect.left + 300, rect.bottom - 15, "FPF", 3 ) ;
		if( this->sm_fit_plot_method == FIT_CURVE_FROC )
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
		if( this->sm_fit_plot_method == FIT_CURVE_ROC )
			dc.TextOutA( rect.left, rect.bottom - 180, "TPF", 3 ) ;
		if( this->sm_fit_plot_method == FIT_CURVE_AFROC )
			dc.TextOutA( rect.left, rect.bottom - 180, "LLF", 3 ) ;
		if( this->sm_fit_plot_method == FIT_CURVE_FROC )
			dc.TextOutA( rect.left, rect.bottom - 180, "LLF", 3 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	rect.left += 10 ;
	rect.bottom -= 10 ;


	float max_x_FROC = UNINITIALIZED ;
	if( this->sm_fit_plot_method == FIT_CURVE_FROC ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < number_of_bins_FROC[ i ][ j ] ; k++ ) {
					if( NLF_BIN_FROC[ i ][ j ][ k ] > max_x_FROC ) max_x_FROC = NLF_BIN_FROC[ i ][ j ][ k ] ;
				}
			}
		}
		max_x_FROC = (float)( (int)( max_x_FROC * 100.0f ) ) / 100.0f + 0.2f ;
	}
	char str[ 200 ] ;

	// axis
	SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( BLACK_PEN ) ) ;
	dc.MoveTo( rect.left, rect.bottom - 10 ) ; dc.LineTo( rect.right, rect.bottom - 10 ) ;
	dc.MoveTo( rect.right - 3, rect.bottom - 15 ) ;	dc.LineTo( rect.right, rect.bottom - 10 ) ;	dc.LineTo( rect.right - 3, rect.bottom - 5 ) ;
	dc.MoveTo( rect.left + 10, rect.bottom ) ;	dc.LineTo( rect.left + 10, rect.top ) ;
	dc.MoveTo( rect.left + 5, rect.top + 3 ) ;	dc.LineTo( rect.left + 10, rect.top ) ;	dc.LineTo( rect.left + 15, rect.top + 3 ) ;
	dc.MoveTo( rect.left + 5, rect.top + 10 ) ; dc.LineTo( rect.left + 15, rect.top + 10 ) ;
	dc.MoveTo( rect.right - 10, rect.bottom - 15 ) ; dc.LineTo( rect.right - 10, rect.bottom - 5 ) ;
	dc.TextOut( rect.left + 20, rect.top, "1.0", 3 ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_FROC )
		sprintf_s( str, "%3.1f", max_x_FROC ) ;
	else 
		sprintf_s( str, "%3.1f", 1.0f ) ;
	dc.TextOut( rect.right - 20, rect.bottom - 30, str, strlen(str) ) ;

	if( this->sm_fit_plot_method == FIT_CURVE_ROC )
		PlotROC( &dc,  rect.left + 10, rect.top + 10, rect.right - 10, rect.bottom - 10 ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_AFROC )
		PlotAFROC( &dc,  rect.left + 10, rect.top + 10, rect.right - 10, rect.bottom - 10 ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_FROC )
		PlotFROC( &dc,  rect.left + 10, rect.top + 10, rect.right - 10, rect.bottom - 10 ) ;

	CStatic *status ;
	sprintf_s( str, "Reader: %s, Modality: %s", readerTable[ fit_plot_reader ], modalityTable[ fit_plot_modality ] ) ;
	status = (CStatic *)GetDlgItem( IDC_FIT_PLOT_LABEL ) ;	status->SetWindowText( str ) ;
	*/

}

void CFitPlotDialog::OnBnClickedFitPlotPrev()
{
	// TODO: Add your control notification handler code here
	Invalidate() ;
}

void CFitPlotDialog::OnBnClickedFitPlotNext()
{
	// TODO: Add your control notification handler code here
	Invalidate() ;
}


void CFitPlotDialog::OnBnClickedSmFitSaveCsv()
{
	// TODO: Add your control notification handler code here
	char filename[ 200 ], str[ 200 ] ;
	FILE *stream ;
	float nlf[ N_TAPS * 2 + 1 ], llf[ N_TAPS * 2 + 1 ] ;
/*
	if( this->sm_fit_plot_method == FIT_CURVE_ROC )
		sprintf_s( filename, "%s_SM_FIT_ROC.csv", selectedfilename ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_AFROC )
		sprintf_s( filename, "%s_SM_FIT_AFROC.csv", selectedfilename ) ;
	if( this->sm_fit_plot_method == FIT_CURVE_FROC )
		sprintf_s( filename, "%s_SM_FIT_FROC.csv", selectedfilename ) ;

	FILE *stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			if( this->sm_fit_plot_method == FIT_CURVE_ROC ) {
				fprintf( stream, "FPF, TPF\n" ) ;
				for( int k = 0 ; k < number_of_bins_ROC[ i ][ j ] ; k++ ) {
					fprintf( stream, "%f, %f\n", FPF_BIN_ROC[ i ][ j ][ k ], TPF_BIN_ROC[ i ][ j ][ k ] ) ;
				}
			}
			if( this->sm_fit_plot_method == FIT_CURVE_AFROC ) {
				fprintf( stream, "FPF, LLF\n" ) ;
				for( int k = 0 ; k < number_of_bins_AFROC[ i ][ j ] ; k++ ) {
					fprintf( stream, "%f, %f\n", FPF_BIN_AFROC[ i ][ j ][ k ], LLF_BIN_AFROC[ i ][ j ][ k ] ) ;
				}
			}
			if( this->sm_fit_plot_method == FIT_CURVE_FROC ) {
				fprintf( stream, "NLF, LLF\n" ) ;
				for( int k = 0 ; k < number_of_bins_FROC[ i ][ j ] ; k++ ) {
					fprintf( stream, "%f, %f\n", NLF_BIN_FROC[ i ][ j ][ k ], LLF_BIN_FROC[ i ][ j ][ k ] ) ;
				}
			}

			fit_plot_reader = j ;
			fit_plot_modality = i ;

			fprintf( stream, "Fitted Curve\n" ) ;
			if( this->sm_fit_plot_method == FIT_CURVE_ROC ) {
				fprintf( stream, "FPF, TPF\n" ) ;
				PlotROCPoints( nlf, llf ) ;
			}
			if( this->sm_fit_plot_method == FIT_CURVE_AFROC ) {
				fprintf( stream, "FPF, LLF\n" ) ;
				PlotAFROCPoints( nlf, llf ) ;
			}
			if( this->sm_fit_plot_method == FIT_CURVE_FROC ) {
				fprintf( stream, "NLF, LLF\n" ) ;
				PlotFROCPoints( nlf, llf ) ;
			}
			for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
				fprintf( stream, "%f, %f\n", nlf[ k ], llf[ k ] ) ;
			}
		}
	}

	fclose( stream ) ;

	CStatic *status ;
	sprintf_s( str, "Saved CSV file: %s", filename ) ;
	status = (CStatic *)GetDlgItem( IDC_FIT_PLOT_LABEL ) ;	status->SetWindowText( str ) ; ;
	*/
	sprintf_s( filename, "%s_SM_FIT_ROC.csv", selectedfilename ) ;

	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			fprintf( stream, "FPF, LLF\n" ) ;
			for( int k = 0 ; k < number_of_bins_ROC[ i ][ j ] ; k++ ) {
				fprintf( stream, "%f, %f\n", FPF_BIN_ROC[ i ][ j ][ k ], TPF_BIN_ROC[ i ][ j ][ k ] ) ;
			}

			fit_plot_reader = j ;
			fit_plot_modality = i ;

			fprintf( stream, "Fitted Curve\n" ) ;
			fprintf( stream, "FPF, TPF\n" ) ;
			PlotROCPoints( nlf, llf ) ;
			for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
				fprintf( stream, "%f, %f\n", nlf[ k ], llf[ k ] ) ;
			}
		}
	}

	fclose( stream ) ;


	sprintf_s( filename, "%s_SM_FIT_AFROC.csv", selectedfilename ) ;

	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			fprintf( stream, "FPF, LLF\n" ) ;
			for( int k = 0 ; k < number_of_bins_AFROC[ i ][ j ] ; k++ ) {
				fprintf( stream, "%f, %f\n", FPF_BIN_AFROC[ i ][ j ][ k ], LLF_BIN_AFROC[ i ][ j ][ k ] ) ;
			}

			fit_plot_reader = j ;
			fit_plot_modality = i ;

			fprintf( stream, "Fitted Curve\n" ) ;
			fprintf( stream, "FPF, LLF\n" ) ;
			PlotAFROCPoints( nlf, llf ) ;
			for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
				fprintf( stream, "%f, %f\n", nlf[ k ], llf[ k ] ) ;
			}
		}
	}

	fclose( stream ) ;



	sprintf_s( filename, "%s_SM_FIT_FROC.csv", selectedfilename ) ;

	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) {
		MessageBox( "Cannot open CSV file for save" ) ;
		return ;
	}

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			fprintf( stream, "Reader, %s, Treatment, %s\n", readerTable[ j ], modalityTable[ i ] ) ;
			fprintf( stream, "Operating Points\n" ) ;
			fprintf( stream, "NLF, LLF\n" ) ;
			for( int k = 0 ; k < number_of_bins_FROC[ i ][ j ] ; k++ ) {
				fprintf( stream, "%f, %f\n", NLF_BIN_FROC[ i ][ j ][ k ], LLF_BIN_FROC[ i ][ j ][ k ] ) ;
			}

			fit_plot_reader = j ;
			fit_plot_modality = i ;

			fprintf( stream, "Fitted Curve\n" ) ;
			fprintf( stream, "NLF, LLF\n" ) ;
			PlotFROCPoints( nlf, llf ) ;
			for( int k = 0 ; k < N_TAPS * 2 + 1 ; k++ ) {
				fprintf( stream, "%f, %f\n", nlf[ k ], llf[ k ] ) ;
			}
		}
	}

	fclose( stream ) ;

}


void CFitPlotDialog::PlotIndividualCurve( CDC &dc, CRect rect, int mode )
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
		if( mode == MODE_FIT_ROC )
			dc.TextOutA( ( rect.right - rect.left ) / 2 - 4 + rect.left, rect.bottom - 15, "FPF", 3 ) ;
		if( mode == MODE_FIT_AFROC )
			dc.TextOutA( ( rect.right - rect.left ) / 2 - 4 + rect.left, rect.bottom - 15, "FPF", 3 ) ;
		if( mode == MODE_FIT_FROC )
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
		if( mode == MODE_FIT_ROC )
			dc.TextOutA( rect.left, ( rect.bottom - rect.top ) / 2 + 5 + rect.top, "TPF", 3 ) ;
		if( mode == MODE_FIT_AFROC )
			dc.TextOutA( rect.left, ( rect.bottom - rect.top ) / 2 + 5 + rect.top, "LLF", 3 ) ;
		if( mode == MODE_FIT_FROC )
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
		if( mode == MODE_FIT_ROC )
			dc.TextOutA( rect.right - 45, rect.bottom - 25, "ROC", 3 ) ;
		if( mode == MODE_FIT_AFROC )
			dc.TextOutA( rect.right - 65, rect.bottom - 25, "AFROC", 5 ) ;
		if( mode == MODE_FIT_FROC )
			dc.TextOutA( rect.right - 55, rect.bottom - 25, "FROC", 4 ) ;
	 
		// Restore dc to previous state    
		dc.RestoreDC( DcRestorePoint );
	}

	// ruler
	if( mode == MODE_FIT_ROC || mode == MODE_FIT_AFROC || mode == MODE_FIT_FROC ) {
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

	if( mode == MODE_FIT_ROC || mode == MODE_FIT_AFROC ) {
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
	if( mode == MODE_FIT_FROC ) {

		float max_x_FROC = -1e15 ;
		if( fit_plot_reader < 0 ) {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				if( fit_plot_modality_arr[ i ] == 0 ) continue ;
				if( froc_fit_avg_parms[ i ][ 0 ] > max_x_FROC ) max_x_FROC = froc_fit_avg_parms[ i ][ 0 ] ;
			}
		} else {
			for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
				if( fit_plot_modality_arr[ i ] == 0 ) continue ;
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					if( fit_plot_reader != j ) continue ;
					if( froc_fit_parms[ i ][ j ][ 0 ] > max_x_FROC ) max_x_FROC = froc_fit_parms[ i ][ j ][ 0 ] ;
				}
			}
		}
		if( max_x_FROC < 0 ) max_x_FROC = 1.0f ;

//		max_x_FROC = (float)( (int)( max_x_FROC * 100.0f ) ) / 100.0f + 0.2f ;

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

	if( mode == MODE_FIT_ROC ) {
		for( int plot_modality = 0 ; plot_modality < N_TREATMENTS ; plot_modality++ ) {
			if( fit_plot_modality_arr[ plot_modality ] == 0 ) continue ;
			SetDCPenColor( dc.GetSafeHdc(), RGB( plot_color_table[ plot_modality % 12 ][ 0 ], plot_color_table[ plot_modality % 12 ][ 1 ], plot_color_table[ plot_modality % 12 ][ 2 ] ) ) ;
			SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
			fit_plot_modality = plot_modality ;
			PlotROC( &dc, rect.left, rect.top, rect.right, rect.bottom ) ;
		}
	}
	if( mode == MODE_FIT_AFROC ) {
		for( int plot_modality = 0 ; plot_modality < N_TREATMENTS ; plot_modality++ ) {
			if( fit_plot_modality_arr[ plot_modality ] == 0 ) continue ;
			SetDCPenColor( dc.GetSafeHdc(), RGB( plot_color_table[ plot_modality % 12 ][ 0 ], plot_color_table[ plot_modality % 12 ][ 1 ], plot_color_table[ plot_modality % 12 ][ 2 ] ) ) ;
			SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
			fit_plot_modality = plot_modality ;
			PlotAFROC( &dc, rect.left, rect.top, rect.right, rect.bottom ) ;
		}
	}
	if( mode == MODE_FIT_FROC ) {
		for( int plot_modality = 0 ; plot_modality < N_TREATMENTS ; plot_modality++ ) {
			if( fit_plot_modality_arr[ plot_modality ] == 0 ) continue ;
			SetDCPenColor( dc.GetSafeHdc(), RGB( plot_color_table[ plot_modality % 12 ][ 0 ], plot_color_table[ plot_modality % 12 ][ 1 ], plot_color_table[ plot_modality % 12 ][ 2 ] ) ) ;
			SelectObject( dc.GetSafeHdc(), (HPEN)GetStockObject( DC_PEN ) ) ;
			fit_plot_modality = plot_modality ;
			PlotFROC( &dc, rect.left, rect.top, rect.right, rect.bottom ) ;
		}
	}
}

void CFitPlotDialog::OnLbnSelchangePlotReader()
{
	// TODO: Add your control notification handler code here
	fit_plot_reader = m_list_reader.GetCurSel() ;
	if( MAX_READERS > 1 ) {
		if( fit_plot_reader == 0 ) {
			// averaged plot
			fit_plot_reader-- ;
			Invalidate() ;
		} else {
			fit_plot_reader-- ;
			Invalidate() ;
		}
	} else {
		Invalidate() ;
	}
}


void CFitPlotDialog::OnLbnSelchangePlotModality()
{
	// TODO: Add your control notification handler code here
	int nCount = m_list_modality.GetSelCount() ;
	CArray<int,int> aryListBoxSel ;

	aryListBoxSel.SetSize( nCount ) ;
	m_list_modality.GetSelItems( nCount, aryListBoxSel.GetData() ) ; 
	
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		fit_plot_modality_arr[ i ] = 0 ;
	}
	for( int i = 0 ; i < nCount ; i++ ) {
		fit_plot_modality_arr[ aryListBoxSel.GetAt( i ) ] = 1 ;
	}

	Invalidate() ;
}

void CFitPlotDialog::WriteSummary( void )
{
	CString strText, strNewText ;

	for( int j = 0 ; j < MAX_READERS ; j++ ) {
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			char str[ 1000 ] ;
			sprintf( str, "Reader: %s, Modality: %s\r\n", readerTable[ j ], modalityTable[ i ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;

			sprintf( str, "ROC Fit Mu: %8.5f, Lambda: %8.5f, Nu: %8.5f\r\n", roc_fit_parms[ i ][ j ][ 2 ], roc_fit_parms[ i ][ j ][ 0 ], roc_fit_parms[ i ][ j ][ 3 ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;
			sprintf( str, "AFROC Fit Mu: %8.5f, Lambda: %8.5f, Nu: %8.5f\r\n", afroc_fit_parms[ i ][ j ][ 2 ], afroc_fit_parms[ i ][ j ][ 0 ], afroc_fit_parms[ i ][ j ][ 3 ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;
			sprintf( str, "FROC Fit Mu: %8.5f, Lambda: %8.5f, Nu: %8.5f\r\n\r\n", froc_fit_parms[ i ][ j ][ 2 ], froc_fit_parms[ i ][ j ][ 0 ], froc_fit_parms[ i ][ j ][ 3 ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;
		}
	}
	if( MAX_READERS > 1 ) {
		strNewText.AppendFormat( _T("%s"), "\r\n" ) ;

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			char str[ 1000 ] ;
			sprintf( str, "Reader: Averaged, Modality: %s\r\n", modalityTable[ i ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;

			sprintf( str, "ROC Fit Mu: %8.5f, Lambda: %8.5f, Nu: %8.5f\r\n", roc_fit_avg_parms[ i ][ 2 ], roc_fit_avg_parms[ i ][ 0 ], roc_fit_avg_parms[ i ][ 3 ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;
			sprintf( str, "AFROC Fit Mu: %8.5f, Lambda: %8.5f, Nu: %8.5f\r\n", afroc_fit_avg_parms[ i ][ 2 ], afroc_fit_avg_parms[ i ][ 0 ], afroc_fit_avg_parms[ i ][ 3 ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;
			sprintf( str, "FROC Fit Mu: %8.5f, Lambda: %8.5f, Nu: %8.5f\r\n\r\n", froc_fit_avg_parms[ i ][ 2 ], froc_fit_avg_parms[ i ][ 0 ], froc_fit_avg_parms[ i ][ 3 ] ) ;
			strNewText.AppendFormat( _T("%s"), str ) ;
		}
	}
	m_param_summary.SetSel( 0xffff, 0xffff ) ;
	m_param_summary.ReplaceSel( strNewText ) ;
}

