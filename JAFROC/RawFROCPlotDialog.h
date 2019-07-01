#pragma once
#include "afxwin.h"
#include "ListBoxCH.h"

// CRawFROCPlotDialog dialog

class CRawFROCPlotDialog : public CDialog
{
	DECLARE_DYNAMIC(CRawFROCPlotDialog)

public:
	CRawFROCPlotDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRawFROCPlotDialog();

// Dialog Data
	enum { IDD = IDD_RAW_FROC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	void PlotIndividualCurve( CDC &dc, CRect rect, int mode ) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedRawFrocPrev();
	afx_msg void OnBnClickedRawFrocNext();
	afx_msg void OnBnClickedSaveCsv();
	afx_msg void OnLbnSelchangeRawPlotReader();
	CListBox m_list_reader;
	CListBoxCH m_list_modality;
	afx_msg void OnLbnSelchangeRawPlotModality();
};
