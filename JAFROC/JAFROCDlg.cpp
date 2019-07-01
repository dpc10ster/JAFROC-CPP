// JAFROCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JAFROC.h"
#include "JAFROCDlg.h"
#include "RawFROCPlotDialog.h"
#include "FitPlotdialog.h"
#include "SampleSizeDialog.h"
#include "common.h"
#include "MenuToolTip.h"
#include <stdio.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "MyStrings.h"

// to handle *.ini file
extern CJAFROCApp theApp ;

// console display routines
CFont consoleFont ;
CEdit *filenameEdit ;
CEdit *consoleEdit ;
CToolBar m_ToolBar, m_ToolBar2, m_ToolBar3, m_ToolBar4, m_ToolBar5, m_ToolBar6;
bool data_ready = false ;

bool if_disclaimer = false ;
bool sample_estimate = false ;

bool if_creat_roc_file = false ;
bool if_pseudovalue_sas_files = false;

int analysis_method , analysis_method_temp , last_analysis_method , method_digits ;
bool perform_bootstrap ;
extern void PrintDataSummaryInitial(FILE *report );
CString selectedfilename, datafilename, fileName, outfilename ;
CMenu *mainmenu, *submenu , *pPopup;


void GetTimeStamp( CString *timeStamp )
{
	SYSTEMTIME systime ;
	GetLocalTime( &systime ) ;

	timeStamp->Format( _T("%d/%d/%d %02d:%02d:%02d\r\n"), 
		systime.wMonth, systime.wDay, systime.wYear,
		systime.wHour, systime.wMinute, systime.wSecond ) ;
}

void WriteToConsole( char *str )
{
	CString strText, strNewText ;
	strNewText.Format( _T("%s"), str ) ;	
	consoleEdit->SetSel( 0xffff ,0xffff) ;
	consoleEdit->ReplaceSel( strNewText ) ;
}


void ClearMemoryBeforeExit( void )
{
	clear_data() ;
	clear_treatment_means() ;
	clear_pseudovalues() ;
	clear_roc_data() ;

	clear_zeta_roc() ;
	clear_zeta_afroc() ;
	clear_zeta_froc() ;

	clear_binned_afroc_data() ;
	clear_binned_roc_data() ;
	clear_binned_froc_data() ;

	clear_parameters_roc() ;
	clear_parameters_afroc() ;
	clear_parameters_froc() ;

	clear_roc_fit_parms() ;
	clear_afroc_fit_parms() ;
	clear_froc_fit_parms() ;

	clear_raw_roc() ;
	clear_raw_afroc() ;
	clear_raw_froc() ;

	if( Left != NULL ) delete[] Left ;
	if( Right != NULL ) delete[] Right ;
	Left = NULL ;
	Right = NULL ;

//	clear_bootstrap() ;
	clear_sample_size_arr() ;

	if( MAX_CASES_LL_sm_fit != NULL ) {
		delete[] MAX_CASES_LL_sm_fit ;
		MAX_CASES_LL_sm_fit = NULL ;
	}

	free_anova_dll() ;
}


bool copyexceltemplatefile( void )
{
	FILE *from, *to ;
	char ch ;

	char source[ 200 ], target[ 200 ] ;
	char sCurDir[ 200 ] ;
//	::GetCurrentDirectory(200,sCurDir);
	::GetModuleFileName(0, sCurDir, 200 ) ;
	CString sourcepath = sCurDir ;
	int pos = 0 ;
	while( sourcepath.Find( '\\', pos ) != -1 ) pos = sourcepath.Find( '\\', pos ) + 1 ;
	sourcepath = sourcepath.Mid( 0, pos ) ;
	sprintf_s( source, "%splot_template.xls", sourcepath ) ;
	CString workingdir = theApp.GetProfileString( "Working Directory", "Directory", sCurDir ) ;
	sprintf_s( target, "%splot_template.xls", workingdir ) ;
	
	if( ( from = fopen( source, "rb" ) ) == NULL ) return false ;
	if( ( to = fopen( target, "wb" ) ) == NULL ) return false ;

	while( !feof( from ) ) {
		ch = fgetc( from ) ;
		if( ferror( from ) ) return false ;
		if( !feof( from ) ) fputc( ch, to ) ;
		if( ferror( to ) ) return false ;
	}

	fclose( from ) ;
	fclose( to ) ;

	return true ;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickIdtOptions();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_COMMAND(IDT_OPTIONS, &CAboutDlg::OnClickIdtOptions)
END_MESSAGE_MAP()


// CJAFROCDlg dialog




CJAFROCDlg::CJAFROCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJAFROCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CJAFROCDlg::~CJAFROCDlg()
{
	ClearMemoryBeforeExit() ;
}

void CJAFROCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CJAFROCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MENUSELECT()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)	
	ON_WM_CREATE()
	ON_NOTIFY(TBN_DROPDOWN,AFX_IDW_TOOLBAR,OnToolbarDropDown)
	//}}AFX_MSG_MAP
	
	ON_BN_CLICKED(IDC_ANALYZE_DATA, &CJAFROCDlg::OnBnClickedAnalyzeData)
	ON_WM_CLOSE()
	ON_WM_DESTROY()

	
	ON_COMMAND(IDM_FILE_SELECTDATAFILE, &CJAFROCDlg::OnMnClickedFileSelectDataFile)
	ON_COMMAND(IDM_FILE_EXIT, &CJAFROCDlg::OnMnClickedFileExit)
	ON_COMMAND(IDM_ROC_HR_AUC, &CJAFROCDlg::OnCheckedRocHrAuc)
	ON_COMMAND(IDM_ROC_SONG_A1_AUC, &CJAFROCDlg::OnCheckedRocSongA1Auc)
	ON_COMMAND(IDM_ROC_SONG_A2_AUC, &CJAFROCDlg::OnCheckedRocSongA2Auc)
	ON_COMMAND(IDM_ROC_HR_SENSITIVITY, &CJAFROCDlg::OnCheckedRocHrSensitivity)
	ON_COMMAND(IDM_ROC_HR_SPECIFICITY, &CJAFROCDlg::OnCheckedRocHrSpecificity)
	ON_COMMAND(IDM_ROC_SONG_A1_SENSITIVITY, &CJAFROCDlg::OnCheckedRocSongA1Sensitivity)
	ON_COMMAND(IDM_ROC_SONG_A1_SPECIFICITY, &CJAFROCDlg::OnCheckedRocSongA1Specificity)
	ON_COMMAND(IDM_FROC_JAFROC1, &CJAFROCDlg::OnMnCheckedFrocJafroc1)
	ON_COMMAND(IDM_FROC_JAFROC1_WEIGHTED, &CJAFROCDlg::OnMnCheckedFrocJafroc1Weighted)
	ON_COMMAND(IDM_FROC_JAFROC2, &CJAFROCDlg::OnMnCheckedFrocJafroc2)
	ON_COMMAND(IDM_FROC_JAFROC2_WEIGHTED, &CJAFROCDlg::OnMnCheckedFrocJafroc2weighted)
	ON_COMMAND(IDM_FROC_MAX_NLF_ANALYSIS, &CJAFROCDlg::OnMnCheckedFrocMaxNlfAnalysis)
	ON_COMMAND(IDM_FROC_MAX_LLF_ANALYSIS, &CJAFROCDlg::OnMnCheckedFrocMaxLlfAnalysis)
	ON_COMMAND(IDM_EXP_TRNSFM_SPECIFICITY, &CJAFROCDlg::OnMnCheckedFrocMaxInferredSpecificityAnalysis)
	ON_COMMAND(IDM_ROI, &CJAFROCDlg::OnMnCheckedAnalysisMethodRoi)
	ON_COMMAND(IDT_OPEN, &CJAFROCDlg::OnClickIdtOpen)
	ON_COMMAND(IDT_RUN, &CJAFROCDlg::OnClickIdtRun)
	ON_COMMAND(IDT_METHODS, &CJAFROCDlg::OnClickIdtMethods)
	ON_COMMAND(IDM_EMPIRICALPLOTS, &CJAFROCDlg::OnClickEmpiricalplots)
	ON_COMMAND(IDM_PARAMETRICPLOTS, &CJAFROCDlg::OnClickParametricplots)
	ON_COMMAND(IDM_PLOTTING, &CJAFROCDlg::OnClickPlotting)
	ON_COMMAND(IDM_SAMPLESIZEESTIMATION, &CJAFROCDlg::OnClickSamplesizeestimation)
	ON_COMMAND(IDM_SAMPLESIZECALCULATOR, &CJAFROCDlg::OnClickSamplesizecalculator)
	ON_COMMAND(IDM_OPTIONS_CREATEROC, &CJAFROCDlg::OnCheckedOptionsCreateroc)
	ON_COMMAND(IDM_OPTIONS_PSEUDOVALUEFILESFORSAS, &CJAFROCDlg::OnCheckedOptionsPseudovaluefilesforsas)
	ON_COMMAND(IDM_HELP_HELP, &CJAFROCDlg::OnClickHelpHelp)
	ON_COMMAND(IDM_HELP_ABOUTJAFROC, &CJAFROCDlg::OnClickHelpAboutjafroc)
	ON_COMMAND(IDM_ANALYZETHEDATA_RUN, &CJAFROCDlg::OnClickedAnalyzethedataRun)
	ON_COMMAND(IDT_HELP, &CJAFROCDlg::OnClickIdtHelp)
	ON_COMMAND(IDM_FROC_CUSTOM_ANALYSIS, &CJAFROCDlg::OnMnCheckedFrocCustomAnalysis)
END_MESSAGE_MAP()


// hjy

void CJAFROCDlg::EnableComponents( bool enable )
{
	if( enable ) {
		if( (MAX_CASES[ 0 ] > 0)){
			if (analysis_method == 0) {
				
				mainmenu = GetMenu();
				
				mainmenu->EnableMenuItem(1 , MF_BYPOSITION | MF_ENABLED);//Enable analysis method menu
				mainmenu->EnableMenuItem(3 , MF_BYPOSITION | MF_ENABLED);//Enable options
				mainmenu->EnableMenuItem(4 , MF_BYPOSITION | MF_ENABLED);//Enable Analyze

				EnableMethodMenu(ANALYSIS_METHOD_HR_ROC) ; EnableMethodMenu(ANALYSIS_METHOD_HR_SENSITIVITY) ; EnableMethodMenu(ANALYSIS_METHOD_HR_SPECIFICITY) ; 
				EnableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1) ; EnableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY) ; EnableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY) ; 
				EnableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A2) ; EnableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A2_SENSITIVITY) ; EnableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A2_SPECIFICITY) ; 
				

				EnableMethodMenu(ANALYSIS_METHOD_JAFROC1) ; EnableMethodMenu(ANALYSIS_METHOD_JAFROC1_WEIGHTED) ; EnableMethodMenu(ANALYSIS_METHOD_JAFROC);
				EnableMethodMenu(ANALYSIS_METHOD_JAFROC_WEIGHTED) ; EnableMethodMenu(ANALYSIS_METHOD_MAX_NLF) ; EnableMethodMenu(ANALYSIS_METHOD_MAX_LLF) ;
				EnableMethodMenu(ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY) ; EnableMethodMenu(ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS) ;


				EnableMethodMenu(ANALYSIS_METHOD_ROI) ; 

				analysis_method = ANALYSIS_METHOD_JAFROC_WEIGHTED;
				CheckMethodMenu(analysis_method);

				MessageBox(HelpStrOnBnClickedAnalysisJafrocWeighted,NULL,MB_OK);
				WriteToConsole( HelpStrOnBnClickedAnalysisJafrocWeighted) ;

				
				submenu = mainmenu -> GetSubMenu(2);
				submenu->EnableMenuItem(0 , MF_BYPOSITION | MF_ENABLED);
				EnableMethodMenu(IDM_SAMPLESIZEESTIMATION) ; //Enable items in Exploring menu


				m_ToolBar2.GetToolBarCtrl().EnableButton(IDT_METHODS, 1);
				m_ToolBar4.GetToolBarCtrl().EnableButton(IDT_OPTIONS, 1);
				m_ToolBar5.GetToolBarCtrl().EnableButton(IDT_RUN , 1);
			}
		} else{		

			mainmenu = GetMenu();
			
			mainmenu->EnableMenuItem(1 , MF_BYPOSITION | MF_ENABLED);//Enable analysis method menu
			mainmenu->EnableMenuItem(3 , MF_BYPOSITION | MF_ENABLED);//Enable options
			
			DisableMethodMenu(ANALYSIS_METHOD_HR_ROC) ; DisableMethodMenu(ANALYSIS_METHOD_HR_SENSITIVITY) ; DisableMethodMenu(ANALYSIS_METHOD_HR_SPECIFICITY) ; 
			DisableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1) ; DisableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY) ; DisableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY) ; 
			DisableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A2) ; DisableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A2_SENSITIVITY) ; DisableMethodMenu(ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY) ; 
				

			EnableMethodMenu(ANALYSIS_METHOD_JAFROC1) ; EnableMethodMenu(ANALYSIS_METHOD_JAFROC1_WEIGHTED) ; DisableMethodMenu(ANALYSIS_METHOD_JAFROC);
			DisableMethodMenu(ANALYSIS_METHOD_JAFROC_WEIGHTED) ; DisableMethodMenu(ANALYSIS_METHOD_MAX_NLF) ; DisableMethodMenu(ANALYSIS_METHOD_MAX_LLF) ;
			DisableMethodMenu(ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY) ; 

			DisableMethodMenu(ANALYSIS_METHOD_ROI) ;

			

			submenu = mainmenu -> GetSubMenu(2);
			submenu->EnableMenuItem(0 , MF_BYPOSITION | MF_ENABLED);
			EnableMethodMenu(IDM_SAMPLESIZEESTIMATION) ; //Enable items in Exploring menu

			

			if ((analysis_method != ANALYSIS_METHOD_JAFROC1) && (analysis_method != ANALYSIS_METHOD_JAFROC1_WEIGHTED)){
				UnCheckMethodMenu(analysis_method);				
				analysis_method = ANALYSIS_METHOD_JAFROC1;
				CheckMethodMenu(analysis_method);
			}

		}
	} else {
		// disable components. will be enabled after loading a data
		mainmenu = GetMenu();
		mainmenu->EnableMenuItem(1 , MF_BYPOSITION | MF_DISABLED);//Disable analysis method menu
		mainmenu->EnableMenuItem(3 , MF_BYPOSITION | MF_DISABLED);//Disable options
		mainmenu->EnableMenuItem(4 , MF_BYPOSITION | MF_DISABLED);//Disable Analyze

		m_ToolBar2.GetToolBarCtrl().EnableButton(IDT_METHODS, 0);
		m_ToolBar4.GetToolBarCtrl().EnableButton(IDT_OPTIONS, 0);
		m_ToolBar5.GetToolBarCtrl().EnableButton(IDT_RUN , 0);

	}
}

void CJAFROCDlg::UnCheckMethodMenu(int analysis_method) //Uncheck the previous method
{
	method_digits = 1;
	analysis_method_temp = analysis_method;
	while (analysis_method_temp/=10)  method_digits++;
	int *menu_item_position = new int[method_digits];
	GetMenuItemPostition(analysis_method , menu_item_position);
	submenu = GetMenu();
	for (int i = 1 ; i < method_digits ; i++){
		submenu = submenu->GetSubMenu(menu_item_position[method_digits - i ]);
	}
	submenu->CheckMenuItem(menu_item_position[0] , MF_BYPOSITION | MF_UNCHECKED);
	delete[] menu_item_position;
}

void CJAFROCDlg::CheckMethodMenu(int analysis_method) //Check the previous method
{
	method_digits = 1;
	analysis_method_temp = analysis_method;
	while (analysis_method_temp/=10)  method_digits++;
	int *menu_item_position = new int[method_digits];
	GetMenuItemPostition(analysis_method , menu_item_position);
	submenu = GetMenu();
	for (int i = 1 ; i < method_digits ; i++){
		submenu = submenu->GetSubMenu(menu_item_position[method_digits - i ]);
	}
	submenu->CheckMenuItem(menu_item_position[0] , MF_BYPOSITION | MF_CHECKED);
	delete[] menu_item_position;
}

void CJAFROCDlg::EnableMethodMenu(int analysis_method) //Enable the previous method
{
	method_digits = 1;
	analysis_method_temp = analysis_method;
	while (analysis_method_temp/=10)  method_digits++;
	int *menu_item_position = new int[method_digits];
	GetMenuItemPostition(analysis_method , menu_item_position);
	submenu = GetMenu();
	for (int i = 1 ; i < method_digits ; i++){
		submenu = submenu->GetSubMenu(menu_item_position[method_digits - i ]);
	}
	submenu->EnableMenuItem(menu_item_position[0] , MF_BYPOSITION | MF_ENABLED);
	delete[] menu_item_position;
}

void CJAFROCDlg::DisableMethodMenu(int analysis_method) //Disable the previous method
{
	method_digits = 1;
	analysis_method_temp = analysis_method;
	while (analysis_method_temp/=10)  method_digits++;
	int *menu_item_position = new int[method_digits];
	GetMenuItemPostition(analysis_method , menu_item_position);
	submenu = GetMenu();
	for (int i = 1 ; i < method_digits ; i++){
		submenu = submenu->GetSubMenu(menu_item_position[method_digits - i ]);
	}
	submenu->EnableMenuItem(menu_item_position[0] , MF_BYPOSITION | MF_DISABLED);
	delete[] menu_item_position;
}

void CJAFROCDlg::UnCheckToolbarMenu(int analysis_method) //Uncheck the previous method
{
	method_digits = 1;
	analysis_method_temp = analysis_method;
	while (analysis_method_temp/=10)  method_digits++;
	int *menu_item_position = new int[method_digits];
	GetMenuItemPostition(analysis_method , menu_item_position);
	for (int i = 2 ; i < method_digits ; i++){
		pPopup = pPopup->GetSubMenu(menu_item_position[method_digits - i ]);
	}
	pPopup->CheckMenuItem(menu_item_position[0] , MF_BYPOSITION | MF_UNCHECKED);
	delete[] menu_item_position;
}

void CJAFROCDlg::CheckToolbarMenu(int analysis_method) //Check the previous method
{
	method_digits = 1;
	analysis_method_temp = analysis_method;
	while (analysis_method_temp/=10)  method_digits++;
	int *menu_item_position = new int[method_digits];
	GetMenuItemPostition(analysis_method , menu_item_position);
	for (int i = 2 ; i < method_digits ; i++){
		pPopup = pPopup->GetSubMenu(menu_item_position[method_digits - i ]);
	}
	pPopup->CheckMenuItem(menu_item_position[0] , MF_BYPOSITION | MF_CHECKED);
	delete[] menu_item_position;
}

void CJAFROCDlg::GetMenuItemPostition(int analysis_method , int menu_item_position[])
{	
	analysis_method_temp = analysis_method;
	for (int i = 1; i <= method_digits; i++){
		menu_item_position[i - 1] = analysis_method_temp % 10;
		analysis_method_temp/=10;
	}	
	menu_item_position[method_digits - 1] -= 1 ; 
}


BOOL CJAFROCDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)  //Tooltips for toolbar
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
 
    // if there is a top level routing frame then let it handle the message
    if (GetRoutingFrame() != NULL) return FALSE;
 
    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;
 
    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
    }
 
    if (nID != 0) // will be zero on a separator
    {
        AfxLoadString(nID, szFullText);
        strTipText=szFullText;
 
#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif 
        *pResult = 0; 
        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);       
        return TRUE;
    } 
    return FALSE;
}


void CJAFROCDlg::OnToolbarDropDown(NMHDR *pNMHDR, LRESULT *plRes)
 {
	
	LPNMTOOLBAR pnmtb = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);
	CWnd *pWnd;
	CMenu menu;
	UINT nID;
	switch(pnmtb->iItem){
		case IDT_METHODS:  
			pWnd = &m_ToolBar2;
			if( (MAX_CASES[ 0 ] > 0)){
				nID = IDR_MENU2;			
			}else{
				nID = IDR_MENU4;
			}
			menu.LoadMenu(nID);
			pPopup = menu.GetSubMenu(0);	
			if (last_analysis_method != 0){
				UnCheckToolbarMenu(last_analysis_method);
			}
			pPopup = menu.GetSubMenu(0);
			CheckToolbarMenu(analysis_method);
			last_analysis_method = analysis_method;			
			break;
		case IDT_OPTIONS:  
			pWnd = &m_ToolBar4;
			nID = IDR_MENU3;			
			menu.LoadMenu(nID);
			pPopup = menu.GetSubMenu(0);
			if (if_creat_roc_file){
				CheckToolbarMenu(IDM_OPTIONS_CREATEROC);
			}
			else{
				UnCheckToolbarMenu(IDM_OPTIONS_CREATEROC);
			}
			if (if_pseudovalue_sas_files){
				CheckToolbarMenu(IDM_OPTIONS_PSEUDOVALUEFILESFORSAS);
			}
			else{
				UnCheckToolbarMenu(IDM_OPTIONS_PSEUDOVALUEFILESFORSAS);
			}
			break;
		case IDT_EXPLORING:  
			pWnd = &m_ToolBar3;
			nID = IDR_MENU5;			
			menu.LoadMenu(nID);
			pPopup = menu.GetSubMenu(0);
			if (data_ready){
				pPopup->EnableMenuItem(0, MF_BYPOSITION | MF_ENABLED);
				pPopup->EnableMenuItem(3, MF_BYPOSITION | MF_ENABLED);
			}
			else{
				pPopup->EnableMenuItem(0, MF_BYPOSITION | MF_DISABLED);
				pPopup->EnableMenuItem(3, MF_BYPOSITION | MF_DISABLED);
			}
			break;
		default:
			return;
	}	
	pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup);
	CRect rc;
	pWnd->SendMessage(TB_GETRECT,pnmtb->iItem,(LPARAM)&rc);
	pWnd->ClientToScreen(&rc);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
		rc.left,rc.bottom,this,&rc);
}

void CJAFROCDlg::CreatToolbars()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

	if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1) ){	//for Windows 7

	m_ToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar	
	m_ToolBar.LoadToolBar(IDR_TOOLBAR1);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);	

	m_ToolBar2.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar2.LoadToolBar(IDR_TOOLBAR2);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//m_ToolBar2.GetToolBarCtrl().EnableButton(IDT_METHODS, 0);
	
	m_ToolBar3.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar3.LoadToolBar(IDR_TOOLBAR3);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ToolBar4.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar4.LoadToolBar(IDR_TOOLBAR4);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//m_ToolBar4.GetToolBarCtrl().EnableButton(IDT_OPTIONS, 0);

	m_ToolBar5.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar5.LoadToolBar(IDR_TOOLBAR5);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ToolBar6.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar6.LoadToolBar(IDR_TOOLBAR6);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar4.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	DWORD dwStyle = m_ToolBar4.GetButtonStyle(m_ToolBar4.CommandToIndex(IDT_OPTIONS));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar4.SetButtonStyle(m_ToolBar4.CommandToIndex(IDT_OPTIONS),dwStyle);
	
	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar2.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	dwStyle = m_ToolBar2.GetButtonStyle(m_ToolBar2.CommandToIndex(IDT_METHODS));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar2.SetButtonStyle(m_ToolBar2.CommandToIndex(IDT_METHODS),dwStyle);	

	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar3.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	dwStyle = m_ToolBar3.GetButtonStyle(m_ToolBar3.CommandToIndex(IDT_EXPLORING));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar3.SetButtonStyle(m_ToolBar3.CommandToIndex(IDT_EXPLORING),dwStyle);
	} 
	
	
	else if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 2) ){  // for Windows 8
	m_ToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar	
	m_ToolBar.LoadToolBar(IDR_TOOLBAR7);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);	

	m_ToolBar2.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar2.LoadToolBar(IDR_TOOLBAR8);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//m_ToolBar2.GetToolBarCtrl().EnableButton(IDT_METHODS, 0);
	
	m_ToolBar3.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar3.LoadToolBar(IDR_TOOLBAR9);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ToolBar4.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar4.LoadToolBar(IDR_TOOLBAR10);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//m_ToolBar4.GetToolBarCtrl().EnableButton(IDT_OPTIONS, 0);

	m_ToolBar5.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar5.LoadToolBar(IDR_TOOLBAR11);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ToolBar6.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar6.LoadToolBar(IDR_TOOLBAR12);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar4.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	DWORD dwStyle = m_ToolBar4.GetButtonStyle(m_ToolBar4.CommandToIndex(IDT_OPTIONS));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar4.SetButtonStyle(m_ToolBar4.CommandToIndex(IDT_OPTIONS),dwStyle);
	
	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar2.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	dwStyle = m_ToolBar2.GetButtonStyle(m_ToolBar2.CommandToIndex(IDT_METHODS));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar2.SetButtonStyle(m_ToolBar2.CommandToIndex(IDT_METHODS),dwStyle);	

	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar3.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	dwStyle = m_ToolBar3.GetButtonStyle(m_ToolBar3.CommandToIndex(IDT_EXPLORING));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar3.SetButtonStyle(m_ToolBar3.CommandToIndex(IDT_EXPLORING),dwStyle);
	} 
	
	
	else {
	m_ToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar	
	m_ToolBar.LoadToolBar(IDR_TOOLBAR13);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);	

	m_ToolBar2.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar2.LoadToolBar(IDR_TOOLBAR14);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//m_ToolBar2.GetToolBarCtrl().EnableButton(IDT_METHODS, 0);
	
	m_ToolBar3.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar3.LoadToolBar(IDR_TOOLBAR15);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ToolBar4.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar4.LoadToolBar(IDR_TOOLBAR16);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//m_ToolBar4.GetToolBarCtrl().EnableButton(IDT_OPTIONS, 0);

	m_ToolBar5.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar5.LoadToolBar(IDR_TOOLBAR17);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ToolBar6.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		CRect(0,0,0,0)); //Creat toolbar
	m_ToolBar6.LoadToolBar(IDR_TOOLBAR18);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar4.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	DWORD dwStyle = m_ToolBar4.GetButtonStyle(m_ToolBar4.CommandToIndex(IDT_OPTIONS));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar4.SetButtonStyle(m_ToolBar4.CommandToIndex(IDT_OPTIONS),dwStyle);
	
	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar2.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	dwStyle = m_ToolBar2.GetButtonStyle(m_ToolBar2.CommandToIndex(IDT_METHODS));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar2.SetButtonStyle(m_ToolBar2.CommandToIndex(IDT_METHODS),dwStyle);	

	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBar3.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle); 
	dwStyle = m_ToolBar3.GetButtonStyle(m_ToolBar3.CommandToIndex(IDT_EXPLORING));
	dwStyle |= TBSTYLE_DROPDOWN; 
	m_ToolBar3.SetButtonStyle(m_ToolBar3.CommandToIndex(IDT_EXPLORING),dwStyle);
	}
}

// CJAFROCDlg message handlers

CMenuToolTip myMenuToolTip;

BOOL CJAFROCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// obtain console handle
	consoleEdit = (CEdit *)GetDlgItem( IDC_CONSOLE ) ;
	consoleFont.CreateFont( 16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, (LPCTSTR)"Courier New" ) ;
	consoleEdit->SetFont( &consoleFont ) ;
	consoleEdit->LimitText(-1); //Limit of console, -1 is the largest
	myMenuToolTip.Create(m_hWnd, _T(""), AfxGetResourceHandle()  //Creat tooltips for menu
			,TTS_NOPREFIX | TTS_BALLOON
			, _T("")
			);
	CreatToolbars();
	
	//filenameEdit = (CEdit *)GetDlgItem( IDC_FILENAME ) ;
	EnableComponents( false ) ;	

	


	
	return TRUE;  // return TRUE  unless you set the focus to a control	
}


void CJAFROCDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSubMenu)
{
	myMenuToolTip.OnMenuSelect(nItemID, nFlags, hSubMenu);
	CWnd::OnMenuSelect(nItemID, nFlags, hSubMenu);
}

void CJAFROCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);		
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJAFROCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();						
		if (if_disclaimer == false){
			PrintDataSummaryInitial( NULL );
			if_disclaimer = true;
		}
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJAFROCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CJAFROCDlg::OnBnClickedRawFroc()
{
	// TODO: Add your control notification handler code here
	if( nlf_raw_froc == NULL ) {
		SetCursor( LoadCursor( 0, IDC_WAIT ) ) ;
		raw_froc_op_points() ;
		raw_afroc_op_points() ;
		raw_roc_op_points() ;
		SetCursor( LoadCursor( 0, IDC_ARROW ) ) ;
	}

	CRawFROCPlotDialog dlgPlot ;
	dlgPlot.DoModal() ;
}


void CJAFROCDlg::OnBnClickedAnalysisBootstrap()
{
	// TODO: Add your control notification handler code here
	if( perform_bootstrap ) {
		perform_bootstrap = false ;
		WriteToConsole( "Perform Jackknife analysis\r\n\r\n" ) ;
	} else {
		perform_bootstrap = true ;
		WriteToConsole( "Perform Bootstrap analysis\r\n\r\n" ) ;
	}
}



void CJAFROCDlg::OnBnClickedAnalyzeData()
{
	// TODO: Add your control notification handler code here
	SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

	// file name composition
	// filenameprefix: contains path name and original file name without extension.
	// outputfilename: original file name without path name
	// outfilename: file name to write data
	CString filenameprefix ;
	filenameprefix = selectedfilename ;
	if( perform_bootstrap ) {
		switch( analysis_method ) {
			case ANALYSIS_METHOD_JAFROC1:
				outfilename = filenameprefix + "_JAFROC1_NP.txt" ;
				break ;
			case ANALYSIS_METHOD_JAFROC:
				outfilename = filenameprefix + "_JAFROC_NP.txt" ;
				break ;
			case ANALYSIS_METHOD_HR_ROC:
				outfilename = filenameprefix + "_ROC_NP.txt" ;
				break ;
			case ANALYSIS_METHOD_HR_SENSITIVITY:
				outfilename = filenameprefix + "_HR_SENSITIVITY.txt" ;
				break ;
			case ANALYSIS_METHOD_HR_SPECIFICITY:
				outfilename = filenameprefix + "_HR_SPECIFICITY.txt" ;
				break ;
			case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
				outfilename = filenameprefix + "_JAFROC1_WEIGHTED_NP.txt" ;
				break ;
			case ANALYSIS_METHOD_JAFROC_WEIGHTED:
				outfilename = filenameprefix + "_JAFROC_WEIGHTED_NP.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1:
				outfilename = filenameprefix + "_Song_A1_AUC.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY:
				outfilename = filenameprefix + "_Song_A1_SENSITIVITY.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY:
				outfilename = filenameprefix + "_Song_A1_SPECIFICITY:.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A2:
				outfilename = filenameprefix + "_Song_A2_AUC.txt" ;
				break ;
			case ANALYSIS_METHOD_ROI:
				outfilename = filenameprefix + "_ROI.txt" ;
				break ;
			case ANALYSIS_METHOD_MAX_NLF:
				outfilename = filenameprefix + "_MAX_NLF.txt" ;
				break ;		
			case ANALYSIS_METHOD_MAX_LLF:
				outfilename = filenameprefix + "_MAX_LLF.txt" ;
				break ;
			case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
				outfilename = filenameprefix + "_EXP_TRNSFM_SPECIFICITY.txt" ;
				break; 
			case ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS:
				outfilename = filenameprefix + "_FROC_CUSTOM_ANALYSIS.txt" ;
				break; }
	} else {
		switch( analysis_method ) {
			case ANALYSIS_METHOD_JAFROC1:
				outfilename = filenameprefix + "_JAFROC1.txt" ;
				break ;
			case ANALYSIS_METHOD_JAFROC:
				outfilename = filenameprefix + "_JAFROC.txt" ;
				break ;
			case ANALYSIS_METHOD_HR_ROC:
				outfilename = filenameprefix + "_Inferred_ROC.txt" ;
				break ;
			case ANALYSIS_METHOD_HR_SENSITIVITY:
				outfilename = filenameprefix + "_HR_SENSITIVITY.txt" ;
				break ;
			case ANALYSIS_METHOD_HR_SPECIFICITY:
				outfilename = filenameprefix + "_HR_SPECIFICITY.txt" ;
				break ;
			case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
				outfilename = filenameprefix + "_JAFROC1_WEIGHTED.txt" ;
				break ;
			case ANALYSIS_METHOD_JAFROC_WEIGHTED:
				outfilename = filenameprefix + "_JAFROC_WEIGHTED.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1:
				outfilename = filenameprefix + "_Song_A1_AUC.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY:
				outfilename = filenameprefix + "_Song_A1_SENSITIVITY.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY:
				outfilename = filenameprefix + "_Song_A1_SPECIFICITY:.txt" ;
				break ;
			case ANALYSIS_METHOD_SONG_FOM_A2:
				outfilename = filenameprefix + "_Song_A2_AUC.txt" ;
				break ;
			case ANALYSIS_METHOD_ROI:
				outfilename = filenameprefix + "_ROI.txt" ;
				break ;
			case ANALYSIS_METHOD_MAX_NLF:
				outfilename = filenameprefix + "_MAX_NLF.txt" ;
				break ;
			case ANALYSIS_METHOD_MAX_LLF:
				outfilename = filenameprefix + "_MAX_LLF.txt" ;
				break ;
			case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
				outfilename = filenameprefix + "_EXP_TRNSFM_SPECIFICITY.txt" ;
				break; 
			case ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS:
				outfilename = filenameprefix + "_FROC_CUSTOM_ANALYSIS.txt" ;
				break; }
	}

	int pos = 0 ;
	while ( outfilename.Find( '\\', pos ) != -1 ) pos = outfilename.Find( '\\', pos ) + 1 ;
	CString outputfilename = outfilename.Mid( pos ) ;

	WriteToConsole( "Analyzing...\r\n\r\n" ) ;

	switch (analysis_method) {
			case ANALYSIS_METHOD_MAX_NLF:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_Normals_DLL() ;
				break;
			case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
				Alloc_DLL_Memory() ;//**dpc 
				jackknife_FROC_Data_Normals_DLL() ;
				break;
			case ANALYSIS_METHOD_MAX_LLF:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_Abnormals_DLL() ;
				break;
			case ANALYSIS_METHOD_HR_SENSITIVITY:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_Abnormals_DLL() ;
				break;
			case ANALYSIS_METHOD_HR_SPECIFICITY:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_Normals_DLL() ;
				break;
			case ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_Normals_DLL() ;
				break;
			case ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_Abnormals_DLL() ;
				break;
			case ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_DLL() ;
				break;
			default:
				Alloc_DLL_Memory() ;
				jackknife_FROC_Data_DLL() ;
				break;
			}	
		
	// Generate report file
	WriteToConsole( "Reporting to file: " ) ;
	WriteToConsole( outfilename.GetBuffer() ) ; outfilename.ReleaseBuffer() ;
	WriteToConsole( "\r\n\r\n" ) ;

	FILE *stream ;
	errno_t err = fopen_s( &stream, outfilename, "wt" ) ;
	if( err != 0 ) {
		WriteToConsole( "File open failure!\r\n\r\n" ) ;
		MessageBox( "Cannot open report file", NULL, MB_OK ) ;
		SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
		return ;
	}
	if( !Data_QC_Check() ) {
		SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
		return ;
	}

	// DBM ANOVA DLL - 6/2/2011
	RUN_ANOVA_DLL(analysis_method) ;
	PrintDataSummary( stream ) ;
	Print_ANOVA_Report( stream ) ;

	// Generate LABMRMC data file
	if( if_creat_roc_file ) {
		CString filename = filenameprefix + "_MRMC.lrc" ;
		WriteToConsole( "Create LABMRMC data file: " ) ;
		WriteToConsole( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
		WriteToConsole( "\r\n" ) ;
		save_mrmc_file( stream, filename.GetBuffer() ) ;
		filename.ReleaseBuffer() ;
	}

	// Generate PseudoValues file
	if( if_pseudovalue_sas_files ) {
		CString filename = filenameprefix + "_PV.txt" ;
		WriteToConsole( "Create pseudovalues file for SAS: " ) ;
		WriteToConsole( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
		WriteToConsole( "\r\n" ) ;
		print_pseudovalues( stream, filename.GetBuffer() ) ;
		filename.ReleaseBuffer() ;
	}

	// sample size estimation
	if( sample_estimate == true ) {
		if( analysis_method != ANALYSIS_METHOD_ROI ) {
		es_roc = 0.05f ;
		ss_alpha = 0.05f ;
		desired_power = 0.8f ;
		if( MAX_READERS > 1 ) {
			SampleSizeMultiReader() ;
		} else {
			SampleSizeSingleReader() ;
		}
		CSampleSizeDialog ssd ;
		if( ssd.DoModal() == IDOK ) {
			SampleSizeReport( stream ) ;
			if( SampleSizeGraph( filenameprefix.GetBuffer() ) ) {
				MessageBox( "Unable to write Sample Size Estimation plot CSV file." ) ;
			}
			filenameprefix.ReleaseBuffer() ;
		}
		} else {
			WriteToConsole( "ROI FOM: Skip sample size estimation process...\r\n\r\n" ) ;
		}
	}

	fprintf( stream, "================================================================================\n" ) ;
	fclose( stream ) ;

	WriteToConsole( "Done.\r\n\r\n" ) ;

	// open notepad to show report
	// removed 10/26/2010
/*
	{
		char command[ 200 ] ;
		sprintf( command, "notepad %s", outfilename ) ;
		system( command ) ;
	}
*/
	SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
}


void CJAFROCDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ClearMemoryBeforeExit() ;

	CDialog::OnClose();
}


void CJAFROCDlg::OnDestroy()
{
	CDialog::OnDestroy();
	ClearMemoryBeforeExit() ;

	// TODO: Add your message handler code here
}


void CJAFROCDlg::BackToPrevButton( void )
{
	CheckDlgButton( IDC_ANALYSIS_JAFROC, BST_UNCHECKED ) ;
	CheckDlgButton( IDC_ANALYSIS_JAFROC1, BST_UNCHECKED ) ;

	CheckDlgButton( IDC_ANALYSIS_ROC, BST_UNCHECKED ) ;

	CheckDlgButton( IDC_ANALYSIS_JAFROC_WEIGHTED, BST_UNCHECKED ) ;
	CheckDlgButton( IDC_ANALYSIS_JAFROC1_WEIGHTED, BST_UNCHECKED ) ;
	CheckDlgButton( IDC_ANALYSIS_MAX_NLF, BST_UNCHECKED ) ;
	CheckDlgButton( IDC_ANALYSIS_LESION_MAX_LLF, BST_UNCHECKED ) ;

	CheckDlgButton( IDC_ANALYSIS_SONG_FOM_A0, BST_UNCHECKED ) ;
	CheckDlgButton( IDC_ANALYSIS_SONG_FOM_A1, BST_UNCHECKED ) ;
	CheckDlgButton( IDC_ANALYSIS_SONG_FOM_A2, BST_UNCHECKED ) ;

	CheckDlgButton( IDC_ANALYSIS_ROI, BST_UNCHECKED ) ;

	switch( analysis_method ) {
	case ANALYSIS_METHOD_JAFROC:
		CheckDlgButton( IDC_ANALYSIS_JAFROC, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_JAFROC1:
		CheckDlgButton( IDC_ANALYSIS_JAFROC1, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_HR_ROC:
		CheckDlgButton( IDC_ANALYSIS_ROC, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_JAFROC_WEIGHTED:
		CheckDlgButton( IDC_ANALYSIS_JAFROC_WEIGHTED, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_MAX_NLF:
		CheckDlgButton( IDC_ANALYSIS_MAX_NLF, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_MAX_LLF:
		CheckDlgButton( IDC_ANALYSIS_LESION_MAX_LLF, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY:
		CheckDlgButton( IDM_EXP_TRNSFM_SPECIFICITY, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
		CheckDlgButton( IDC_ANALYSIS_JAFROC1_WEIGHTED, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_SONG_FOM_A0:
		CheckDlgButton( IDC_ANALYSIS_SONG_FOM_A0, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_SONG_FOM_A1:
		CheckDlgButton( IDC_ANALYSIS_SONG_FOM_A1, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_SONG_FOM_A2:
		CheckDlgButton( IDC_ANALYSIS_SONG_FOM_A2, BST_CHECKED ) ;
		break ;
	case ANALYSIS_METHOD_ROI:
		CheckDlgButton( IDC_ANALYSIS_ROI, BST_CHECKED ) ;
		break ;
	}
}


void CJAFROCDlg::OnMnClickedFileSelectDataFile()
{
	// TODO: Add your command handler code here
	char strFilter[] = { "JAFROC Data Files (*.xls*)|*.xls*|JAFROC V1 Files (*.txt)|*.txt||" };
	CFileDialog fileDialog( TRUE, (LPCTSTR)".xls", NULL, 0, (LPCTSTR)strFilter ) ;
	char sCurDir[200];
//	::GetCurrentDirectory(200,sCurDir);
	::SHGetSpecialFolderPath( NULL, sCurDir, CSIDL_PERSONAL, FALSE ) ;
	CString workingdir = theApp.GetProfileString( "Working Directory", "Directory", sCurDir ) ;
	fileDialog.m_pOFN->lpstrInitialDir = workingdir ;
	if( fileDialog.DoModal() == IDOK ) {
		CString folderpath = fileDialog.GetPathName() ;
		int pos = 0 ;
		while( folderpath.Find( '\\', pos ) != -1 ) pos = folderpath.Find( '\\', pos ) + 1 ;
		folderpath = folderpath.Mid( 0, pos ) ;
		theApp.WriteProfileString( "Working Directory", "Directory", folderpath ) ;
		if( data_ready ) ClearMemoryBeforeExit() ;	// unload previous data if exist
		fileName = fileDialog.GetPathName() ;		
		if( !read_jafroc_data( fileName.GetBuffer(0) ) ) {
//			MessageBox( "Error reading JAFROC daafile" ) ;
			data_ready = false ;
			EnableComponents( false ) ;
			return ;
		}
		data_ready = true ;
		//filenameEdit->SetWindowText( fileDialog.GetFileName() ) ;
		datafilename = fileDialog.GetFileName() ;

		EnableComponents( true ) ;

		selectedfilename = fileDialog.GetPathName().SpanExcluding(".") ;
	}
}


void CJAFROCDlg::OnMnClickedFileExit()
{
	// TODO: Add your command handler code here
	ClearMemoryBeforeExit() ;
	EndDialog(IDOK);
}


void CJAFROCDlg::OnCheckedRocHrAuc()
{
	if( analysis_method != ANALYSIS_METHOD_HR_ROC ) {
		UnCheckMethodMenu(analysis_method);
		analysis_method = ANALYSIS_METHOD_HR_ROC ;
		CheckMethodMenu(analysis_method);
		WriteToConsole( HelpStrOnBnClickedAnalysisRoc) ;
		MessageBox(HelpStrOnBnClickedAnalysisRoc,NULL,MB_OK);
	}	
}


void CJAFROCDlg::OnCheckedRocHrSensitivity()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_HR_SENSITIVITY ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_HR_SENSITIVITY ;
		CheckMethodMenu(analysis_method);
		WriteToConsole(HelpStrOnMnCheckedHrSensitivity ) ;
		MessageBox(HelpStrOnMnCheckedHrSensitivity,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnCheckedRocHrSpecificity()
{
	if( analysis_method != ANALYSIS_METHOD_HR_SPECIFICITY ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_HR_SPECIFICITY ;
		CheckMethodMenu(analysis_method);
		WriteToConsole(HelpStrOnMnCheckedHrSpecificity ) ;
		MessageBox(HelpStrOnMnCheckedHrSpecificity,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnCheckedRocSongA1Auc()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_SONG_FOM_A1 ) {
		UnCheckMethodMenu(analysis_method);
		analysis_method = ANALYSIS_METHOD_SONG_FOM_A1 ;
		CheckMethodMenu(analysis_method);
		WriteToConsole( helpStrOnBnClickedAnalysisSongFomA1 ) ;
		MessageBox(helpStrOnBnClickedAnalysisSongFomA1,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnCheckedRocSongA1Sensitivity()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY ) {
		UnCheckMethodMenu(analysis_method);
		analysis_method = ANALYSIS_METHOD_SONG_FOM_A1_SENSITIVITY ;
		CheckMethodMenu(analysis_method);
		WriteToConsole( helpStrOnBnClickedAnalysisSongFomA1Sensitivity ) ;
		MessageBox(helpStrOnBnClickedAnalysisSongFomA1Sensitivity,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnCheckedRocSongA1Specificity()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY ) {
		UnCheckMethodMenu(analysis_method);
		analysis_method = ANALYSIS_METHOD_SONG_FOM_A1_SPECIFICITY ;
		CheckMethodMenu(analysis_method);
		WriteToConsole( helpStrOnBnClickedAnalysisSongFomA1Specificity ) ;
		MessageBox(helpStrOnBnClickedAnalysisSongFomA1Specificity,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnCheckedRocSongA2Auc()
{
	if( analysis_method != ANALYSIS_METHOD_SONG_FOM_A2 ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_SONG_FOM_A2 ;
		CheckMethodMenu(analysis_method);
		WriteToConsole( helpStrOnBnClickedAnalysisSongFomA2 ) ;
		MessageBox(helpStrOnBnClickedAnalysisSongFomA2,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocJafroc1()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_JAFROC1 ) {
		UnCheckMethodMenu(analysis_method);
		analysis_method = ANALYSIS_METHOD_JAFROC1 ;		
		CheckMethodMenu(analysis_method);
//		WriteToConsole( "Set analysis method: JAFROC1\r\n\r\n" ) ;
		WriteToConsole( HelpStrOnBnClickedAnalysisJafroc1) ;
		MessageBox(HelpStrOnBnClickedAnalysisJafroc1,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocJafroc1Weighted()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_JAFROC1_WEIGHTED ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_JAFROC1_WEIGHTED ;
		CheckMethodMenu(analysis_method);
//		WriteToConsole( "Set analysis method: JAFROC1\r\n\r\n" ) ;
		WriteToConsole( HelpStrOnBnClickedAnalysisJafroc1Weighted ) ;
		MessageBox(HelpStrOnBnClickedAnalysisJafroc1Weighted,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocJafroc2()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_JAFROC ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_JAFROC ;
		CheckMethodMenu(analysis_method);
//		WriteToConsole( "Set analysis method: JAFROC\r\n\r\n" ) ;
		WriteToConsole(HelpStrOnBnClickedAnalysisJafroc ) ;
		MessageBox(HelpStrOnBnClickedAnalysisJafroc,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocJafroc2weighted()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_JAFROC_WEIGHTED ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_JAFROC_WEIGHTED ;
		CheckMethodMenu(analysis_method);
//		WriteToConsole( "Set analysis method: JAFROC\r\n\r\n" ) ;
		WriteToConsole( HelpStrOnBnClickedAnalysisJafrocWeighted) ;
		MessageBox(HelpStrOnBnClickedAnalysisJafrocWeighted,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocMaxNlfAnalysis()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_MAX_NLF ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_MAX_NLF ;
		CheckMethodMenu(analysis_method);
		WriteToConsole(HelpStrOnBnClickedAnalysisMaxNLF ) ;
		MessageBox(HelpStrOnBnClickedAnalysisMaxNLF,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocMaxLlfAnalysis()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_MAX_LLF ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_MAX_LLF ;
		CheckMethodMenu(analysis_method);
		WriteToConsole(HelpStrOnBnClickedAnalysisMaxLLF ) ;
		MessageBox(HelpStrOnBnClickedAnalysisMaxLLF,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedAnalysisMethodRoi()
{
	// TODO: Add your command handler code here
	if( analysis_method != ANALYSIS_METHOD_ROI ) {
		int max_nl = 0 ;
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					int nnl = 0 ;
					for( int l = 0 ; l < MAX_NL ; l++ ) {
						if( NL[ i ][ j ][ k ][ l ] != UNINITIALIZED ) nnl++ ;
					}
					if( nnl > max_nl ) max_nl = nnl ;
				}
			}
		}

		if( max_nl < MAX_LL ) {
			//WriteToConsole( "More True Positives than the number or regions.\r\nNot an appropriate ROI data file.\r\n\r\n" ) ;
			//BackToPrevButton() ;
			return ;
		}

		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 0 ] ; k++ ) {
					for( int l = 0 ; l < max_nl ; l++ ) {
						if( NL[ i ][ j ][ k ][ l ] == UNINITIALIZED ) {
							char str[ 200 ] ;
							//sprintf( str, "Not enough false positive entry for Reader %s Modality %s.\r\n\r\n", 
							//	readerTable[ j ], modalityTable[ i ] ) ;
							//WriteToConsole( str ) ;
							WriteToConsole( "Not an appropriate ROI data file.\r\n\r\n" ) ;
							MessageBox("Not an appropriate ROI data file.\r\n\r\n",NULL,MB_OK);
							//BackToPrevButton() ;
							return ;
						}
					}
				}
			}
		}
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				for( int k = 0 ; k < MAX_CASES[ 1 ] ; k++ ) {
					for( int l = 0 ; l < N_Lesions_Per_Image[ k ] ; l++ ) {
						if( LL[ i ][ j ][ k ][ l ] == UNINITIALIZED ) {
							char str[ 200 ] ;
							//sprintf( str, "Not enough true positive entry for Reader %s Modality %s.\r\n\r\n", 
							//	readerTable[ j ], modalityTable[ i ] ) ;
							//WriteToConsole( str ) ;
							WriteToConsole( "Not an appropriate ROI data file.\r\n\r\n" ) ;
							MessageBox("Not an appropriate ROI data file.\r\n\r\n",NULL,MB_OK);
							//BackToPrevButton() ;
							return ;
						}
					}
					for( int l = 0 ; l < max_nl - N_Lesions_Per_Image[ k ] ; l++ ) {
						if( NL[ i ][ j ][ k + MAX_CASES[ 0 ] ][ l ] == UNINITIALIZED ) {
							char str[ 200 ] ;
							//sprintf( str, "Not enough false positive entry for Reader %s Modality %s.\r\n\r\n", 
							//	readerTable[ j ], modalityTable[ i ] ) ;
							//WriteToConsole( str ) ;
							WriteToConsole( "Not an appropriate ROI data file.\r\n\r\n" ) ;
							MessageBox("Not an appropriate ROI data file.\r\n\r\n",NULL,MB_OK);
							//BackToPrevButton() ;
							return ;
						}
					}
				}
			}
		}
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_ROI ;
		CheckMethodMenu(analysis_method);
		WriteToConsole( helpStrANALYSIS_METHOD_ROI ) ;
		MessageBox(helpStrANALYSIS_METHOD_ROI,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnMnCheckedFrocMaxInferredSpecificityAnalysis()
{
	if( analysis_method != ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY ) {
		UnCheckMethodMenu(analysis_method);		
		analysis_method = ANALYSIS_METHOD_EXP_TRNSFM_SPECIFICITY ;
		CheckMethodMenu(analysis_method);
		WriteToConsole(HelpStrOnMnCheckedExpTransformSpecificityAnalysis ) ;
		MessageBox(HelpStrOnMnCheckedExpTransformSpecificityAnalysis,NULL,MB_OK);
	}
}


void CJAFROCDlg::OnClickEmpiricalplots()
{
	// TODO: Add your command handler code here
	if( nlf_raw_froc == NULL ) {
		SetCursor( LoadCursor( 0, IDC_WAIT ) ) ;
		raw_froc_op_points() ;
		raw_afroc_op_points() ;
		raw_roc_op_points() ;
		SetCursor( LoadCursor( 0, IDC_ARROW ) ) ;
	}

	CRawFROCPlotDialog dlgPlot ;
	dlgPlot.DoModal() ;
}


void CJAFROCDlg::OnClickParametricplots()
{
	// TODO: Add your command handler code here
	SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;
	CString outfilename, filenameprefix ;
	filenameprefix = selectedfilename ;
	CString filename ;
	if( roc_fit_parms == NULL ) {
		sm_roc_ml_fit( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
	}
	if( afroc_fit_parms == NULL ) {
		sm_afroc_ml_fit( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
	}
	if( froc_fit_parms == NULL ) {
		sm_froc_ml_fit( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
	}
		filename = filenameprefix + "_ROC_PLOTS.txt" ;
		excel_plotting_routine_roc( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
		filename = filenameprefix + "_AFROC_PLOTS.txt" ;
		excel_plotting_routine_afroc( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
		filename = filenameprefix + "_FROC_PLOTS.txt" ;
		excel_plotting_routine_froc( filename.GetBuffer() ) ; filename.ReleaseBuffer() ;
		SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
		copyexceltemplatefile() ;

	CFitPlotDialog plotDialog ;
	plotDialog.sm_fit_plot_method = FIT_CURVE_FROC ;
	plotDialog.DoModal() ;
}


void CJAFROCDlg::OnClickPlotting()
{
	// TODO: Add your command handler code here
	ShellExecute(this->m_hWnd, "open", "Operating Characteristics.exe", "", "", SW_SHOW);
}


void CJAFROCDlg::OnClickSamplesizeestimation()
{
	// TODO: Add your command handler code here
	sample_estimate = true;
	OnBnClickedAnalyzeData();
	sample_estimate = false;
}


void CJAFROCDlg::OnClickSamplesizecalculator()
{
	// TODO: Add your command handler code here
	ShellExecute(this->m_hWnd, "open", "Sample Size Calculator.exe", "", "", SW_SHOW);
}


void CJAFROCDlg::OnCheckedOptionsCreateroc()
{
	// TODO: Add your command handler code here	
	if( !if_creat_roc_file  ) {
		CheckMethodMenu(IDM_OPTIONS_CREATEROC);
		if_creat_roc_file = true;
		WriteToConsole( "Generate ROC datafile compatible to DBM MRMC.\r\n\r\n" ) ;
	} else {
		UnCheckMethodMenu(IDM_OPTIONS_CREATEROC);
		if_creat_roc_file = false;
		WriteToConsole( "Do not generate ROC datafile.\r\n\r\n" ) ;
	}
}


void CJAFROCDlg::OnCheckedOptionsPseudovaluefilesforsas()
{
	// TODO: Add your command handler code here
	if( !if_pseudovalue_sas_files ) {
		CheckMethodMenu(IDM_OPTIONS_PSEUDOVALUEFILESFORSAS);
		if_pseudovalue_sas_files = true;
		WriteToConsole( "Generate Pseudovalue file for SAS.\r\n\r\n" ) ;
	} else {
		UnCheckMethodMenu(IDM_OPTIONS_PSEUDOVALUEFILESFORSAS);
		if_pseudovalue_sas_files = false;
		WriteToConsole( "Do not generate Pseudovalue file.\r\n\r\n" ) ;
	}
}


void CJAFROCDlg::OnClickHelpHelp()
{
	// TODO: Add your command handler code here
	ShellExecute(this->m_hWnd, "open", "http://www.devchakraborty.com/", "", "", SW_SHOW);
}


void CJAFROCDlg::OnClickHelpAboutjafroc()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CJAFROCDlg::OnClickIdtOpen()
{
	// TODO: Add your command handler code here
	OnMnClickedFileSelectDataFile();
}


void CJAFROCDlg::OnClickIdtRun()
{
	// TODO: Add your command handler code here
	OnBnClickedAnalyzeData();
}


void CJAFROCDlg::OnClickIdtMethods()
{
	// TODO: Add your command handler code here
}


void CAboutDlg::OnClickIdtOptions()
{
	// TODO: Add your command handler code here
}





void CJAFROCDlg::OnClickedAnalyzethedataRun()
{
	// TODO: Add your command handler code here
	OnBnClickedAnalyzeData();
}


void CJAFROCDlg::OnClickIdtHelp()
{
	// TODO: Add your command handler code here
	OnClickHelpHelp();
}


void CJAFROCDlg::OnMnCheckedFrocCustomAnalysis()
{
	
	if( analysis_method != ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS ) {
		UnCheckMethodMenu(analysis_method);	
		analysis_method = ANALYSIS_METHOD_FROC_CUSTOM_ANALYSIS ;
		CheckMethodMenu(analysis_method);
		WriteToConsole(HelpStrOnMnCheckedFrocCustomAnalysis ) ;
		MessageBox(HelpStrOnMnCheckedFrocCustomAnalysis,NULL,MB_OK);
	}
}
