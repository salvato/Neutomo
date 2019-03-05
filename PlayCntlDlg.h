#pragma once
#include "afxext.h"
#include "afxwin.h"

#ifndef MSG_PLAYCNTL
#define MSG_PLAYCNTL WM_USER
#endif

#define MSG_PLAY       MSG_PLAYCNTL
#define MSG_STOP       MSG_PLAYCNTL + 1
#define MSG_PREV       MSG_PLAYCNTL + 2
#define MSG_NEXT       MSG_PLAYCNTL + 3
#define MSG_CLOSE_PLAY MSG_PLAYCNTL + 4

// finestra di dialogo CPlayCntlDlg

class 
CPlayCntlDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayCntlDlg)

public:
	CPlayCntlDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CPlayCntlDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_PLAYCNTL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  CWnd* pMyParent;
	int xControl, yControl;

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedPlay();
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedStop();
  afx_msg void OnBnClickedPrev();
  afx_msg void OnBnClickedNext();
  CButton buttonPlay;
  CButton buttonStop;
  CButton buttonBack;
  CButton buttonNext;
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnClose();
};
