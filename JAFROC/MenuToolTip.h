#ifndef _MENU_TOOLTIP
#define _MENU_TOOLTIP

#ifndef TTS_NOANIMATE
	#define TTS_NOANIMATE           0x10
	#define TTS_NOFADE              0x20
	#define TTS_BALLOON             0x40
	#define TTS_CLOSE               0x80

	#define TTM_SETTITLEA           (WM_USER + 32)  // wParam = TTI_*, lParam = char* szTitle
	#define TTM_SETTITLEW           (WM_USER + 33)  // wParam = TTI_*, lParam = wchar* szTitle

	#ifdef UNICODE
	#define TTM_SETTITLE            TTM_SETTITLEW
	#else
	#define TTM_SETTITLE            TTM_SETTITLEA
	#endif
#endif


class CMenuToolTip
{

  public:
	CMenuToolTip():m_hToolTip(0), m_hParent(0) {}

	// Create the Tooltip to associate the text
	void Create(HWND hParent, LPCTSTR sczTipText, 
			HINSTANCE hInstance = NULL, 
			DWORD dwStyle = 0,
			LPCTSTR sczTipTitle = NULL);
	// Call this funtions on WM_MENUSELECT of menu owner windw
	// with LOWORD(wParam), HIWORD(wParam), lParam
	void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSubMenu);

	// Show or hide tooltip
	void ShowToolTip(BOOL bShow)
	{
		TOOLINFO    ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND;
		ti.hwnd   = m_hParent;
		ti.uId    = (UINT)m_hParent;
		::SendMessage(m_hToolTip,TTM_TRACKACTIVATE,(WPARAM)bShow,(LPARAM)&ti);
	}

	// Set Tip Position
	void SetToolTipPosition(HMENU hMenu, UINT nItemID)
	{
		RECT rt = {0,0,0,0};
		
		// find Item Rectangle and position
		for(int nItem = 0; nItem < ::GetMenuItemCount(hMenu); nItem++) {
			UINT cmd = ::GetMenuItemID(hMenu, nItem);
			if(cmd == nItemID) {
				::GetMenuItemRect(NULL, hMenu, nItem, &rt);
			}
		}
		
		
		// move position
		::SendMessage(m_hToolTip, TTM_TRACKPOSITION, 0,
						(LPARAM)MAKELPARAM(rt.right + 10, rt.top + 2));
		// make it top most
		::SetWindowPos(m_hToolTip, HWND_TOPMOST ,0,0,0,0, SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
	}

	// Update tiptext
	void UpdateToolTipText(LPCTSTR szBuff, HINSTANCE hInstance = 0)
	{

		TOOLINFO    ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND;
		ti.hwnd   = m_hParent;
		ti.uId    = (UINT)m_hParent;
		ti.hinst = hInstance;
		ti.lpszText  = const_cast<LPTSTR>(szBuff);
		::SendMessage(m_hToolTip,TTM_UPDATETIPTEXT,(WPARAM)0,(LPARAM)&ti);
	}

	operator HWND()
	{
		return m_hToolTip;
	}

  private:
		HWND m_hToolTip;
		HWND m_hParent;
		TCHAR m_szDefault[_MAX_PATH] ;
		HINSTANCE m_hInstance;
};

inline
void CMenuToolTip::Create(HWND hParent, LPCTSTR sczTipText, HINSTANCE hInstance, 
							DWORD dwStyle, LPCTSTR sczTipTitle)
{
	INITCOMMONCONTROLSEX icex;
	TOOLINFO    ti;
	// Load the ToolTip class from the DLL.
	icex.dwSize = sizeof(icex);
	icex.dwICC  = ICC_BAR_CLASSES;

	if(!InitCommonControlsEx(&icex))
	   return;

	m_hParent = hParent;
	m_hInstance = hInstance;
	   
	// Create the ToolTip control.
	m_hToolTip = ::CreateWindow(TOOLTIPS_CLASS, TEXT(""),
						  WS_POPUP| dwStyle,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  NULL, (HMENU)NULL, hInstance,
						  NULL);

	// Prepare TOOLINFO structure for use as tracking ToolTip.
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_TRACK | TTF_ABSOLUTE;//TTF_CENTERTIP
	ti.hwnd   = hParent;
	ti.uId    = (UINT)hParent;
	ti.hinst  = hInstance;
	ti.lpszText  = const_cast<LPTSTR>(sczTipText);
	if(sczTipText != LPSTR_TEXTCALLBACK) {
		if(sczTipText) {
			_tcscpy(m_szDefault, sczTipText);
		}
		else {
			_tcscpy(m_szDefault, _T("No Text associated"));
		}
	}

	ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	// Add the tool to the control, displaying an error if needed.
	::SendMessage(m_hToolTip,TTM_ADDTOOL,0,(LPARAM)&ti);

	::SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, 300);
	if(sczTipTitle) {
		::SendMessage(m_hToolTip, TTM_SETTITLE, 1, (LPARAM)sczTipTitle);
	}
}

inline 
void CMenuToolTip::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSubMenu)
{
	if(nFlags & MF_POPUP 
		|| (nFlags == 0xFFFF && hSubMenu == NULL)) {	// menu closing 
		ShowToolTip(FALSE);
	}

	if(!(nFlags & MF_POPUP)) {
		
		// Set the position
		SetToolTipPosition(hSubMenu, nItemID);

		// Upodate the text of the tool tip
		TCHAR szBuff[256];
		szBuff[0] = 0;
		int nRet = ::LoadString(m_hInstance, nItemID, szBuff, 256);
#if 0
		for(int i = 0; i < nRet; i++)
		{
			if(szBuff[i] == _T('\n'))
			{
				szBuff[i] = 0;
				break;
			}
		}
#endif		
		if(szBuff[0] != 0) {
			UpdateToolTipText(szBuff);
		}
		else {
			UpdateToolTipText(m_szDefault);
		}
		ShowToolTip(TRUE);
	}
}

#endif _MENU_TOOLTIP