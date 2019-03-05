#pragma once
#include "afxwin.h"


// finestra di dialogo CPreProcessDlg

class 
CPreProcessDlg : public CDialog {

	DECLARE_DYNAMIC(CPreProcessDlg)

public:
	CPreProcessDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CPreProcessDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_PREPROCESS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  bool ChooseDir(bool bEnableCreate, CString& sDir, CString sName, CString sTitle, CString sHint);
  BOOL UpdateData(BOOL bSaveAndValidate);

  virtual BOOL OnInitDialog();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedChangeDarkDir();
  afx_msg void OnBnClickedChangeBeamDir();
  afx_msg void OnBnClickedChangeProjDir();
  afx_msg void OnBnClickedChangeNormDir();
  afx_msg void OnBnClickedUseDark();
  afx_msg void OnBnClickedUseBeam();
  afx_msg void OnBnClickedFilterDark();
  afx_msg void OnBnClickedFilterBeam();
  afx_msg void OnBnClickedPreMedian();
  afx_msg void OnBnClickedPostMedain();
  afx_msg void OnBnClickedAndorBug();
	DECLARE_MESSAGE_MAP()

  CEdit editDarkDir;
  CEdit editBeamDir;
  CEdit editProjDir;
  CEdit editNormDir;

  CEdit editAngularStep;
  CEdit editPostThreshold;

  CButton buttonUseDark;
  CButton buttonUseBeam;
  CButton buttonFilterDark;
  CButton buttonFilterBeam;
  CButton buttonChangeDarkDir;
  CButton buttonChangeBeamDir;
  CButton buttonPostMedianFilter;
  CButton buttonPreMedianFilter;
  CButton buttonPostGaussianFilter;
  CButton buttonAndorBug;

public:

  CString sDarkDir;
  CString sBeamDir;
  CString sProjectionDir;
  CString sNormDir;

  void SetDarkDir(CString sNewDirectory);
  void SetBeamDir(CString sNewDirectory);
  void SetProjectionDir(CString sNewDirectory);
  void SetNormDir(CString sNewDirectory);

  CString GetDarkDir();
  CString GetBeamDir();
  CString GetProjectionDir();
  CString GetNormDir();
  int GetProjectionFormat();
  bool GetPreMedianFilter();
  bool GetPostMedianFilter();

  bool bUseBeam;
  bool bFilterBeam;
  bool GetUseBeam();
  bool GetFilterBeam();
  bool GetCorrectionAndorBug();

  bool bUseDark;
  bool bFilterDark;
  bool GetUseDark();
  bool GetFilterDark();

  bool bPreMedian, bPostMedian, bPostGaussian;
  bool bCorrectAndorBug;

  int ProjectionFormat;

  float fAngularStep;
  float fPostThreshold;
};
