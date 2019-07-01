#pragma once
#include "afxwin.h"
#include "ListBoxCH.h"


// CFitPlotDialog dialog

class CFitPlotDialog : public CDialog
{
	DECLARE_DYNAMIC(CFitPlotDialog)

public:
	CFitPlotDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFitPlotDialog();

// Dialog Data
	enum { IDD = IDD_FIT_PLOT };

public:
	int sm_fit_plot_method ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	void PlotIndividualCurve( CDC &dc, CRect rect, int mode ) ;
	void WriteSummary( void ) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedFitPlotPrev();
	afx_msg void OnBnClickedFitPlotNext();
	afx_msg void OnBnClickedSmFitSaveCsv();
	CListBox m_list_reader;
	CListBoxCH m_list_modality;
	afx_msg void OnLbnSelchangePlotReader();
	afx_msg void OnLbnSelchangePlotModality();
	CEdit m_param_summary;
};
