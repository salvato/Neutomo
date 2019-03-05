#pragma once
#include "afxwin.h"


// finestra di dialogo CSelectShowDirDlg

class CSelectShowDirDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectShowDirDlg)

public:
	CSelectShowDirDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CSelectShowDirDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_SHOWSLICES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  bool ChooseDir(bool bEnableCreate, CString& sDir, CString sName, CString sTitle, CString sHint);
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedChangeShowDir();
  CString sShowDir;
  CEdit editShowDir;
  CString GetShowDir();
};
