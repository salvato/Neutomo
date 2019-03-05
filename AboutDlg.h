// AboutDlg.h : header file
//

#pragma once


//////////////////////////
//   CAboutDlg dialog   //
//////////////////////////

class
CAboutDlg : public CDialog {

public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};
