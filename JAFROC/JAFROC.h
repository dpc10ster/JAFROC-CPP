// JAFROC.h : main header file for the PROJECT_NAME application
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CJAFROCApp:
// See JAFROC.cpp for the implementation of this class
//

class CJAFROCApp : public CWinApp
{
public:
	CJAFROCApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHighestratinginferredrocfomAuc();
};

extern CJAFROCApp theApp;
