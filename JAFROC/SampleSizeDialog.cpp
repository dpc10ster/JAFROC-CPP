// SampleSizeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "JAFROC.h"
#include "SampleSizeDialog.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CSampleSizeDialog dialog

IMPLEMENT_DYNAMIC(CSampleSizeDialog, CDialogEx)

CSampleSizeDialog::CSampleSizeDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSampleSizeDialog::IDD, pParent)
{
	ssd_first = 1 ;
}

CSampleSizeDialog::~CSampleSizeDialog()
{
}

void CSampleSizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SS_MODALITY, modalitySelect);
	DDX_Control(pDX, IDC_ES_ROC, ES_ROC);
	DDX_Control(pDX, IDC_SS_SUMMARY, SS_SUMMARY);
	DDX_Control(pDX, IDC_SS_ALPHA, ssAlpha);
	DDX_Control(pDX, IDC_SS_DESIRED_POWER, SS_DESIRED_POWER);
}


BEGIN_MESSAGE_MAP(CSampleSizeDialog, CDialogEx)
	ON_BN_CLICKED(IDC_SS_RECALCULATE, &CSampleSizeDialog::OnBnClickedSsRecalculate)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CSampleSizeDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CSampleSizeDialog message handlers

void CSampleSizeDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
	if( ssd_first ) {
		ssd_first = 0 ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			modalitySelect.AddString( modalityTable[ i ] ) ;
		}
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			modalitySelect.SetCheck( i, sample_size_modality[ i ] ) ;
		}
		char str[ 80 ] ;
		sprintf( str, "%g", es_roc ) ;
		ES_ROC.SetWindowTextA( str ) ;

		sprintf( str, "%g", ss_alpha ) ;
		ssAlpha.SetWindowTextA( str ) ;

		sprintf( str, "%g", desired_power ) ;
		SS_DESIRED_POWER.SetWindowTextA( str ) ;

		MakeSampleSizeSummary() ;
	}
}


void CSampleSizeDialog::ClearSummaryText( void )
{
	SS_SUMMARY.SetWindowTextA( "" ) ;
}

void CSampleSizeDialog::AddSummaryText( char *str )
{
	CString strNewText ;


	strNewText.Format( _T("%s"), str ) ;
	SS_SUMMARY.SetSel( 0xffff, 0xffff ) ;
	SS_SUMMARY.ReplaceSel( strNewText ) ;
}

extern int sse_sm_param ;

void CSampleSizeDialog::MakeSampleSizeSummary( void )
{
	if( ssd_success == 0 ) {
		AddSummaryText( "Unable to estimate sample size under given conditions. Contact Dr. Chakraborty with the data. \r\n" ) ;
		return ;
	}
	char str[ 200 ] ;

	int hit = 0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			switch( sse_sm_param ) {
			case 1:
				if( n_parameters_ROC[ i ][ j ] == 3 ) hit = 1 ;
				break ;
			case 2:
				if( n_parameters_AFROC[ i ][ j ] == 3 ) hit = 1 ;
				break ;
			case 3:
				if( n_parameters_FROC[ i ][ j ] == 3 ) hit = 1 ;
				break; 
			}
		}
	}

	if( sse_sm_param > 1 ) {
		if( sse_sm_param == 2 ) {
			sprintf( str, " Note: Search model parameters were derived from fitting the AFROC data.\r\n" ) ; AddSummaryText( str ) ;
			if( hit ) {
				sprintf( str, " Warning: Some readers yielded only one AFROC operating point, which may result in unreliable estimates of SM parameters.\r\n" ) ; AddSummaryText( str ) ;
			}
		}
		if( sse_sm_param == 3 ) {
			sprintf( str, " Note: Search model parameters were derived from fitting the FROC data.\r\n" ) ; AddSummaryText( str ) ;
			if( hit ) {
				sprintf( str, " Warning: Some readers yielded only one FROC operating point, which may result in unreliable estimates of SM parameters.\r\n" ) ; AddSummaryText( str ) ;
			}
		}
		sprintf( str, "\r\n" ) ; AddSummaryText( str ) ;
	} else {
		if( hit ) {
			sprintf( str, " Warning: Some readers yielded only one ROC operating point,\r\n" ) ; AddSummaryText( str ) ;
			sprintf( str, "          which may result in unreliable estimates of parameters.\r\n" ) ; AddSummaryText( str ) ;
		}
		sprintf( str, "\r\n" ) ; AddSummaryText( str ) ;
	}


	sprintf( str, "NH ROC FOM: %g\r\n", rocfom_nh ) ; AddSummaryText( str ) ;
	sprintf( str, "AH ROC FOM: %g\r\n", rocfom_ah ) ; AddSummaryText( str ) ;
	sprintf( str, "ROC Effect Size: %g\r\n\r\n", rocfom_ah - rocfom_nh ) ; AddSummaryText( str ) ;

	if( analysis_method != ANALYSIS_METHOD_HR_ROC &&
		analysis_method != ANALYSIS_METHOD_SONG_FOM_A1 &&
		analysis_method != ANALYSIS_METHOD_SONG_FOM_A2 ) {
		sprintf( str, "NH JAFROC FOM: %g\r\n", jafrocfom_nh ) ; AddSummaryText( str ) ;
		sprintf( str, "AH JAFROC FOM: %g\r\n", jafrocfom_ah ) ; AddSummaryText( str ) ;
		sprintf( str, "JAFROC Effect Size: %g\r\n\r\n", jafrocfom_ah - jafrocfom_nh ) ; AddSummaryText( str ) ;
	}

	sprintf( str, "Alpha: %g\r\n", ss_alpha ) ; AddSummaryText( str ) ;
	sprintf( str, "Desired power: %g\r\n\r\n", desired_power ) ; AddSummaryText( str ) ;

	sprintf( str, "Search Model parameters\r\n" ) ; AddSummaryText( str ) ;
	sprintf( str, "NH mu\t: %g\r\n", nh_roc_mu ) ; AddSummaryText( str ) ;
	sprintf( str, "NH lambda: %g\r\n", nh_roc_lambda ) ; AddSummaryText( str ) ;
	sprintf( str, "NH nu\t: %g\r\n\r\n", nh_roc_nu ) ; AddSummaryText( str ) ;

	sprintf( str, "AH mu\t: %g\r\n", ah_roc_mu ) ; AddSummaryText( str ) ;
	sprintf( str, "AH lambda: %g\r\n", ah_roc_lambda ) ; AddSummaryText( str ) ;
	sprintf( str, "AH nu\t: %g\r\n\r\n", ah_roc_nu ) ; AddSummaryText( str ) ;

	sprintf( str, "Prediction\r\n" ) ; AddSummaryText( str ) ;
	if( MAX_READERS > 1 ) {
		for( int i = 0 ; i < 3 ; i++ ) {
			switch( i ) {
			case 0:
				AddSummaryText( "Random All\r\n" ) ;
				break ;
			case 1:
				AddSummaryText( "Random Cases\r\n" ) ;
				break ;
			case 2:
				AddSummaryText( "Random Readers\r\n" ) ;
				break ;
			}
			for( int j = 2 ; j <= MAX_SS_READERS ; j++ ) {
//				if( sample_size_arr[ i ][ j ] < MAX_NCASES_80 && sample_size_arr[ i ][ j ] >= MIN_NCASES_80 ) {

					int si = j ;
					int k ;
					for( k = j + 1 ; k <= MAX_SS_READERS ; k++ ) {
						if( sample_size_arr[ i ][ j ] != sample_size_arr[ i ][ k ] ) break ;
					}
					int ei = k ;

					if( sample_size_arr[ i ][ j ] >= MAX_NCASES_80 ) {
						if( ei - 1 > 2 ) sprintf( str, "# Readers: ~%d\t# Cases: >= %d\r\n", ei - 1, MAX_NCASES_80 ) ;
						else sprintf( str, "# Readers: %d\t# Cases: >= %d\r\n", ei - 1, MAX_NCASES_80 ) ;
					} else if( sample_size_arr[ i ][ j ] < MIN_NCASES_80 ) {
						sprintf( str, "# Readers: %d~\t# Cases: < %d\r\n", si, MIN_NCASES_80 ) ;
					} else {
						if( ei - si > 1 ) { 
							sprintf( str, "# Readers: %d~%d\t# Cases: %d\r\n", si, ei - 1, sample_size_arr[ i ][ j ] ) ;
						} else {
							sprintf( str, "# Readers: %d\t# Cases: %d\r\n", j, sample_size_arr[ i ][ j ] ) ;
						}
					}
					j = ei - 1 ;

//					sprintf( str, "# Readers: %d\t# Cases: %d\r\n", j, sample_size_arr[ i ][ j ] ) ;
					AddSummaryText( str ) ;
//				}
			}
			AddSummaryText( "\r\n" ) ;
		}
	} else {
		if( sample_size_arr[ 0 ][ 0 ] < MAX_NCASES_80 && sample_size_arr[ 0 ][ 0 ] >= MIN_NCASES_80 ) {
			sprintf( str, "Number of cases: %d\r\n", sample_size_arr[ 0 ][ 0 ] ) ;
		} else {
			sprintf( str, "Number of cases: Unable to find.\r\n" ) ;
		}
		AddSummaryText( str ) ;
	}
}


int isNumeric( char *source )
{
	int hit = 0 ;
	for( int i = 0 ; i < strlen( source ) ; i++ ) {
		if( source[ i ] == 0x20 ||
			source[ i ] == 0x2D ||
			source[ i ] == 0x2E ||
			source[ i ] == 0x45 ||
			source[ i ] == 0x65 ||
			( source[ i ] >= 0x30 && source[ i ] <= 0x39 ) ) {
		} else {
			hit = 1 ;
		}
	}
	return hit ;
}


void CSampleSizeDialog::OnBnClickedSsRecalculate()
{
	// TODO: Add your control notification handler code here
	char str[ 200 ] ;

	ES_ROC.GetWindowTextA( str, 200 ) ;
	float es = (float)atof( str ) ;
	if( es <= 0.0f || isNumeric( str ) ) {
		MessageBox( "Invalid Effect Size" ) ;
		return ;
	}

	ssAlpha.GetWindowTextA( str, 200 ) ;
	float alpha = (float)atof( str ) ;
	if( alpha <= 0.0f || isNumeric( str ) ) {
		MessageBox( "Invalid Alpha" ) ;
		return ;
	}

	SS_DESIRED_POWER.GetWindowTextA( str, 200 ) ;
	float dp = (float)atof( str ) ;
	if( dp <= 0.0f || dp >= 1.0f || isNumeric( str ) ) {
		MessageBox( "Invalid Desired Power" ) ;
		return ;
	}

	int mod = 0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		mod += modalitySelect.GetCheck( i ) ;
	}
	if( mod == 0 ) {
		MessageBox( "Select modalities want to include for sample size estimation." ) ;
		return ;
	} else 	{
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			sample_size_modality[ i ] = modalitySelect.GetCheck( i ) ;
		}
		if( es > 0.0f && alpha > 0.0f && dp > 0.0f && dp < 1.0f ) {
			es_roc = es ;
			ss_alpha = alpha ;
			desired_power = dp ;

			if( MAX_READERS > 1 ) {
				SampleSizeMultiReader() ;
			} else {
				SampleSizeSingleReader() ;
			}

			ClearSummaryText() ;
			MakeSampleSizeSummary() ;

			sprintf( str, "%g", es_roc ) ;
			ES_ROC.SetWindowTextA( str ) ;
		}
	}
}


void CSampleSizeDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
