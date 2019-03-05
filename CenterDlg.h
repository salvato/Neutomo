#pragma once
#include "afxwin.h"


// finestra di dialogo CCenterDlg

class 
CCenterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCenterDlg)

public:
	CCenterDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CCenterDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_CENTER_DIALOG };
  CEdit editCenter;
  CEdit editTilt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  virtual BOOL OnInitDialog();
  afx_msg void OnEnChangeCenterPos();
  afx_msg void OnEnChangeTiltAngle();

	DECLARE_MESSAGE_MAP()

protected:
  CWnd* pmyParent;
  void SaveDlgPos();
  void ReadDlgPos();
  int xControl, yControl;
  CString sString;
  double Center;
  double Tilt;

public:
  void SetCenter(double center);
  void SetTilt(double tilt);
  double GetCenter();
  double GetTilt();
  afx_msg void OnBnClickedOk();
};
