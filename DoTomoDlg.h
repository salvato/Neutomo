#pragma once
#include "afxwin.h"
#include "GeometryDlg.h"


// finestra di dialogo CDoTomoDlg

class 
CDoTomoDlg : public CDialog {
	DECLARE_DYNAMIC(CDoTomoDlg)

public:
	CDoTomoDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CDoTomoDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_DOTOMO_DIALOG };

protected:
  BOOL UpdateData(BOOL bSaveAndValidate=TRUE);
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  bool ChooseDir(bool bEnableCreate, CString& sDir, CString sName, CString sTitle, CString sHint);
  CGeometryDlg GeometryDlg;
  FILTER_TYPES FilterType;
  CString sNormDir;
  CString sSliceDir;
  bool bMedianSinograms;

public:
  virtual BOOL OnInitDialog();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedChangeNormDir();
  afx_msg void OnBnClickedChangeSliceDir();
  afx_msg void OnBnClickedSheppLogan();
  afx_msg void OnBnClickedRamLak();
  afx_msg void OnBnClickedFilterSinograms();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedSetGeometry();

  CEdit editNormDir;
  CEdit editSliceDir;
  CButton buttonMedianSinograms;
  CButton buttonSheepLogan;
  CButton buttonRamLak;
  CButton buttonSetGeomtery;

  CString GetNormDir();
  CString GetSliceDir();
  bool GetFilterSinograms();
  FILTER_TYPES GetFilterType();
  BEAM_GEOMETRY GetBeamGeometry();
  float GetSourceSampleDistance();
  float GetSourceDetectorDistance();
  float GetVerticalFOV();
  float GetHorizzontalFOV();
  int   GetVolXSize();
  int   GetVolYSize();
  int   GetVolZSize();

	DECLARE_MESSAGE_MAP()
};
