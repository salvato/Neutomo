// NeuTomoDlg.h : header file

// This Program requires the CG toolkit (ver. 3.0 or better)from NVIDIA.
// Please download the required setup file from the NVIDIA site.

// This program use the shared library version of GLEW.
// You need to copy the headers and libraries into their destination directories. 
// On Windows this typically boils down to copying: 

// bin/glew32.dll     to     %SystemRoot%/system32 
// lib/glew32.lib     to     {VC Root}/Lib 
// include/GL/glew.h  to     {VC Root}/Include/GL 
// include/GL/wglew.h to     {VC Root}/Include/GL 

// where {VC Root} is the Visual C++ root directory, 
// typically C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK 

// You need also GLUT for Win32 version 3.7.6 or greater - The OpenGL Utility Toolkit
// You need to copy the headers and libraries into their destination directories. 

// glut32.dll     to     %SystemRoot%/system32 
// glut32.lib     to     {VC Root}/Lib 
// glut.h         to     {VC Root}/Include/GL 


#pragma once


#include "afxwin.h"
#include "Plot3.h"
#include "MsgWindow.h"
#include "PreProcessDlg.h"
#include "DoTomoDlg.h"
#include "SelectShowDirDlg.h"
#include "Projection.h"
#include "ximage.h"
#include <GL/glew.h>
#include "GL/wglew.h"
#include <GL/glut.h>
#include <Cg/cgGL.h>

class CAlignWnd;
class CCenterDlg;
class CGrabberWnd;
class CVolumeWnd;
class C3DToolsDlg;
class CPlayCntlDlg;
class CVolumeWnd;
class CRoiDlg;

////////////////////////////
//   CNeuTomoDlg dialog   //
////////////////////////////

class
CNeuTomoDlg : public CDialog {

// Construction
public:
	CNeuTomoDlg(CWnd* pParent = NULL);	// standard constructor
  virtual ~CNeuTomoDlg();

// Dialog Data
	enum { IDD = IDD_NEUTOMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
  //CString sNormDir, sSliceDir, sProjDir, sBeamDir, sDarkDir;// Working Directories
  CString sFilePrefix; // Working Files Prefix
  int nTextures;
  void SaveDlgPos();
  void ReadDlgPos();
  int xControl, yControl;
  CString sVersion;

protected:
  //CPlotWindow*      pPlot;          // The Plot Window
  CVolumeWnd*       pVolumeWnd;
  CMsgWindow*       pMsg;            // A Window for the program Messages
  CRoiDlg*          pRoiDlg;         // The Dialog to choose Region of Interest
  CCenterDlg*       pCenterDlg;
  CPreProcessDlg    PreProcessDlg;   // The Dialog for the Projections Pre-Processing
  CSelectShowDirDlg SelectShowDirDlg;// The Dialog to Choose the Show Directory
  C3DToolsDlg*      pDlg3DTools;     //
  CPlayCntlDlg*     pPlayCntlDlg;
  CDoTomoDlg        DoTomoDlg;       // The Dialog to choose the Tomography Output Directory

  int iCurrentSliceShown;
  float grayMin, grayMax;

  CString* pPathNames;    // The Projection Files
  float *pAngles;         // The Collecting Angles
  int nProjections;       // The Projection Number
  int imgHeight, imgWidth;// The Projection Dimensions
  int iEscape;

protected:
  bool bOpenBeamCorr;
  bool CheckGL();
  bool CreateTextures();
  void ShowProjection(CGrabberWnd* pWnd, CProjection* pImg);
  bool CheckAndDeleteFiles(CString sPath);
  bool BuildImgSum(CString sDirectory, CProjection* pImgSum);

	// Generated message map functions
	virtual BOOL OnInitDialog();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

  afx_msg void OnBnClickedPreProcess();
  afx_msg void OnBnClickedDoTomo();
  afx_msg void OnBnClickedShowSlices();
  afx_msg void OnBnClickedMakeNewPhantom();
  afx_msg void OnBnClicked3DShow();

  afx_msg LRESULT OnSelectionDone(WPARAM, LPARAM);
  afx_msg LRESULT OnSelectionChanged(WPARAM, LPARAM);
  
  afx_msg LRESULT OnRedChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGreenChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnBlueChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnIsoChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnShowFrame(WPARAM wParam, LPARAM lParam);

  afx_msg LRESULT OnStatusMessage(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnCloseVolWnd(WPARAM wParam, LPARAM lParam);

  afx_msg LRESULT OnPlaySlices(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnStopSlices(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnPreviousSlice(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnNextSlice(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnClosePlay(WPARAM wParam, LPARAM lParam);

  afx_msg LRESULT OnAlignDone(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnAlignChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnAlignAborted(WPARAM wParam, LPARAM lParam);

  afx_msg LRESULT OnTomoDone(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
  CButton buttonPreProcess;
  CButton buttonDoTomo;
  CButton button3DShow;
  CButton buttonShowSlices;

  CGrabberWnd* pGrabberWnd;     // The Window for Images
  CAlignWnd* pAlignWnd;
  CEdit editStatus;
  CString sStatus;
	HANDLE	hThread; //Elaboration thread
  void DisableButtons();
  void DisableStopButton();
  void EnableButtons();
  void EnableStopButton();
  bool ReadProjections(CString sInputDirectory);
  void ShowStatusMsg(CString sMsg);
  void SetEscape(int iEscape);

public:
  bool bDoTomoEnabled, b3DEnabled;
  CButton buttonExit;
  CButton buttonMakePhantom;
  afx_msg void OnBnClickedCancel();
  CProjection** pProjections;// The Projections array
  float fMin, fMax;
  FILTER_TYPES GetFilterType(void);
  bool GetFilterSinograms(void);
  CString GetSliceDir(void);
  BEAM_GEOMETRY GetBeamGeometry(void);
  CString* GetPathNames(void);
  float* GetAngles(void);
  int GetNProjections(void);
  int GetImgHeight(void);
  int GetImgWidth(void);
  int GetCurrentSlice(void);
  void SetCurrentSlice(int iSlice);
  float GetGrayMin(void);
  float GetGrayMax(void);
  int GetEscape(void);
  float GetSourceObjectDistance(void);
  float GetSourceDetectorDistance(void);
  float GetDetectorWidth(void);
  float GetDetectorHeight(void);
  int   GetVolXSize();
  int   GetVolYSize();
  int   GetVolZSize();
  float rotationCenter;
  float tiltAngle;

protected:
  int EvaluateShift(CProjection* pP1, CProjection* pP2);
  bool FindCenter(CProjection* pProjection0, CProjection* pProjection180);
};
