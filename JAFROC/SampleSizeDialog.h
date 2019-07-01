#pragma once
#include "afxwin.h"


// CSampleSizeDialog dialog

class CSampleSizeDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSampleSizeDialog)

public:
	CSampleSizeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSampleSizeDialog();

// Dialog Data
	enum { IDD = IDD_SAMPLE_SIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CCheckListBox modalitySelect;
	afx_msg void OnBnClickedSsRecalculate();
	CEdit ES_ROC;
	CEdit SS_SUMMARY;
	CEdit ssAlpha;
	afx_msg void OnPaint();

private:
	int ssd_first ;
public:
	void ClearSummaryText( void ) ;
	void AddSummaryText( char *str ) ;
	void MakeSampleSizeSummary( void ) ;
	CEdit SS_DESIRED_POWER;
	afx_msg void OnBnClickedOk();
};
