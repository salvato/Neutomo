#pragma once
#include "afxwin.h"
#include "resource.h"


// finestra di dialogo CSetIntervalDlg

class CSetIntervalDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetIntervalDlg)

public:
	CSetIntervalDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CSetIntervalDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_SET_INTERVAL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  CWnd* pmyParent;

	DECLARE_MESSAGE_MAP()
public:
  CPoint myPoint;
  CEdit editXMin;
  CEdit editXMax;
	int	XMax;
	int	XMin;
  afx_msg void OnBnClickedAccept();
  afx_msg void OnBnClickedCheck();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnEnChangeXMin();
  afx_msg void OnEnChangeXMax();
  CButton buttonCheck;
  virtual BOOL OnInitDialog();
};
