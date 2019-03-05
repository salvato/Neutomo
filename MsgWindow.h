/////////////////////////////////////////////////////////////////////////////
// MsgWindow.h : header file
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MSGWINDOW_H__8B50A9CE_BC72_11D4_88DD_00801E0328BA__INCLUDED_)
#define AFX_MSGWINDOW_H__8B50A9CE_BC72_11D4_88DD_00801E0328BA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define IDM_CLEARTEXT 102

/////////////////////////////////////////////////////////////////////////////
// CMsgWindow window
/////////////////////////////////////////////////////////////////////////////

class
CMsgWindow : public CWnd {
// Construction
public:
	CMsgWindow(CString Title=_T("Messges"));

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ClearText();
	CEdit* pMessages;
	void AddText(CString Text);
	virtual ~CMsgWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgWindow)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CFont* pmyFont;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGWINDOW_H__8B50A9CE_BC72_11D4_88DD_00801E0328BA__INCLUDED_)
