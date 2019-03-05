#pragma once
#include "afxwin.h"

// finestra di dialogo CRoiDlg

class
CRoiDlg : public CDialog {
	DECLARE_DYNAMIC(CRoiDlg)

public:
	CRoiDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CRoiDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_ROIDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
  CWnd* pmyParent;

public:
	CEdit	m_edYMin;
	CEdit	m_edYMax;
	CEdit	m_edXMax;
	CEdit	m_edXMin;
  CEdit editMean;
  CEdit editSigma;
  CButton buttonApply;

	int	m_XMax;
	int	m_XMin;
	int	m_YMax;
	int	m_YMin;
  
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();

  afx_msg void OnEnChangeXmin();
  afx_msg void OnEnChangeXmax();
  afx_msg void OnEnChangeYmin();
  afx_msg void OnEnChangeYmax();
  afx_msg void OnBnClickedApply();
  CRect myRect;
  CString sTitle;

  CButton buttonCancel;
};
