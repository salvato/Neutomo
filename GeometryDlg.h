#pragma once
#include "afxwin.h"


// finestra di dialogo CGeometryDlg

class CGeometryDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeometryDlg)

public:
	CGeometryDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CGeometryDlg();
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedParallelBeam();
  afx_msg void OnBnClickedConeBeam();

// Dati della finestra di dialogo
	enum { IDD = IDD_GEOMETRY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
  BOOL UpdateData(BOOL bSaveAndValidate=TRUE);
  BEAM_GEOMETRY BeamGeometry;
  float SourceSampleDistance, SourceDetectorDistance, HorizzontalFOV, VerticalFOV;
  int VolXSize, VolYSize, VolZSize;
  CString sString;

	DECLARE_MESSAGE_MAP()

public:
  CEdit editSourceSampleDist;
  CEdit editSourceDetectorDist;
  CEdit editHorizzontalFOV;
  CEdit editVerticalFOV;
  CEdit editVolXSize;
  CEdit editVolYSize;
  CEdit editVolZSize;
  CButton buttonParallelBeam;
  CButton buttonConeBeam;

  BEAM_GEOMETRY GetBeamGeometry();
  float GetSourceSampleDistance();
  float GetSourceDetectorDistance();
  float GetVerticalFOV();
  float GetHorizzontalFOV();
  int   GetVolXSize();
  int   GetVolYSize();
  int   GetVolZSize();
};
