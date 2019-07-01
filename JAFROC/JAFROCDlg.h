// JAFROCDlg.h : header file
//


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// CJAFROCDlg dialog
class CJAFROCDlg : public CDialog
{
// Construction
public:
	CJAFROCDlg(CWnd* pParent = NULL);	// standard constructor
	~CJAFROCDlg() ;

// Dialog Data
	enum { IDD = IDD_JAFROC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

//	hjy
protected:
	void EnableComponents( bool enable ) ;
	void BackToPrevButton( void ) ;
	void UnCheckMethodMenu(int analysis_method);
	void CheckMethodMenu(int analysis_method);
	void EnableMethodMenu(int analysis_method);
	void DisableMethodMenu(int analysis_method);
	void UnCheckToolbarMenu(int analysis_method);
	void CheckToolbarMenu(int analysis_method);
	void GetMenuItemPostition(int analysis_method , int menu_item_position[]);
	void CreatToolbars();

// Implementation
protected:
	HICON m_hIcon;
	CToolTipCtrl mouse_tip;

	// Generated message map functions
	virtual BOOL OnInitDialog();	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSubMenu);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT* plRes);
public:
	
	afx_msg void OnBnClickedAnalyzeData();
	afx_msg void OnBnClickedOptionsCreateroc();
	afx_msg void OnBnClickedOptionsPseudovalues();
	afx_msg void OnBnClickedOptionsCreateplots2();
	afx_msg void OnBnClickedRawFroc();
	afx_msg void OnBnClickedAnalysisBootstrap();
	afx_msg void OnBnClickedPlotRoc();
	afx_msg void OnBnClickedPlotAfroc();
	afx_msg void OnBnClickedPlotFroc();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedEstimationPerform();

	afx_msg void OnMnClickedFileSelectDataFile();
	afx_msg void OnMnClickedFileExit();
	/*
	afx_msg void OnMnCheckedRocHighestRatingInferredRoc();
	afx_msg void OnMnCheckedRocSongA1InferredRoc();
	afx_msg void OnMnCheckeedRocSongA2InferredRoc();
	*/
	afx_msg void OnCheckedRocHrAuc();
	afx_msg void OnCheckedRocHrSensitivity();
	afx_msg void OnCheckedRocHrSpecificity();
	afx_msg void OnCheckedRocSongA1Auc();	
	afx_msg void OnCheckedRocSongA1Sensitivity();
	afx_msg void OnCheckedRocSongA1Specificity();
	afx_msg void OnCheckedRocSongA2Auc();	
	afx_msg void OnMnCheckedFrocJafroc1();
	afx_msg void OnMnCheckedFrocJafroc1Weighted();
	afx_msg void OnMnCheckedFrocJafroc2();
	afx_msg void OnMnCheckedFrocJafroc2weighted();
	afx_msg void OnMnCheckedFrocMaxNlfAnalysis();
	afx_msg void OnMnCheckedFrocMaxLlfAnalysis();
	afx_msg void OnMnCheckedAnalysisMethodRoi();
	afx_msg void OnMnCheckedFrocMaxInferredSpecificityAnalysis();
	
	afx_msg void OnClickIdtOpen();
	afx_msg void OnClickIdtRun();	
	afx_msg void OnClickIdtMethods();
	afx_msg void OnClickEmpiricalplots();
	afx_msg void OnClickParametricplots();
	afx_msg void OnClickPlotting();
	afx_msg void OnClickSamplesizeestimation();
	afx_msg void OnClickSamplesizecalculator();
	afx_msg void OnCheckedOptionsCreateroc();
	afx_msg void OnCheckedOptionsPseudovaluefilesforsas();
	afx_msg void OnClickHelpHelp();
	afx_msg void OnClickHelpAboutjafroc();
	afx_msg void OnClickedAnalyzethedataRun();
	afx_msg void OnClickIdtHelp();
	afx_msg void OnMnCheckedFrocCustomAnalysis();
};