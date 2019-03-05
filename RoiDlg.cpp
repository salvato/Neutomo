// RoiDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "resource.h"
#include ".\roidlg.h"


// finestra di dialogo CRoiDlg

IMPLEMENT_DYNAMIC(CRoiDlg, CDialog)
CRoiDlg::CRoiDlg(CWnd* pParent /*=NULL*/)
  : pmyParent(pParent)
  , CDialog(CRoiDlg::IDD, pParent) 
  , sTitle(_T("ROI Limits"))
{
	m_XMax  = 1;
	m_XMin  = 0;
	m_YMax  = 1;
	m_YMin  = 0;
  myRect = CRect(m_XMin, m_XMax, m_YMin, m_YMax);
}


CRoiDlg::~CRoiDlg() {
}


void 
CRoiDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_XMIN,    m_edXMin);
  DDX_Control(pDX, IDC_XMAX,    m_edXMax);
  DDX_Control(pDX, IDC_YMIN,    m_edYMin);
  DDX_Control(pDX, IDC_YMAX,    m_edYMax);
  DDX_Control(pDX, IDC_MEANVAL, editMean);
  DDX_Control(pDX, IDC_SIGMA,   editSigma);
  DDX_Control(pDX, IDC_APPLY,   buttonApply);
  DDX_Text(pDX, IDC_XMIN, m_XMin);
  DDX_Text(pDX, IDC_XMAX, m_XMax);
  DDX_Text(pDX, IDC_YMIN, m_YMin);
  DDX_Text(pDX, IDC_YMAX, m_YMax);
  DDX_Control(pDX, IDCANCEL, buttonCancel);
}


BEGIN_MESSAGE_MAP(CRoiDlg, CDialog)
  ON_BN_CLICKED(IDOK,      OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL,  OnBnClickedCancel)
  ON_EN_CHANGE(IDC_XMIN,   OnEnChangeXmin)
  ON_EN_CHANGE(IDC_XMAX,   OnEnChangeXmax)
  ON_EN_CHANGE(IDC_YMIN,   OnEnChangeYmin)
  ON_EN_CHANGE(IDC_YMAX,   OnEnChangeYmax)
  ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
END_MESSAGE_MAP()


// gestori di messaggi CRoiDlg

BOOL
CRoiDlg::OnInitDialog() {
	CDialog::OnInitDialog();
  SetWindowText(sTitle);
  buttonApply.EnableWindow(false);
	return TRUE;
}


void 
CRoiDlg::OnBnClickedOk() {
  UpdateData(true);
  if(m_XMin > m_XMax) {
    int tmp = m_XMax;
    m_XMax = m_XMin;
    m_XMin = tmp;
  }
  if(m_YMin > m_YMax) {
    int tmp = m_YMax;
    m_YMax = m_YMin;
    m_YMin = tmp;
  }
  OnOK();
}


void
CRoiDlg::OnBnClickedCancel() {
  OnCancel();
}


void
CRoiDlg::OnEnChangeXmin() {
  buttonApply.EnableWindow(true);
}


void
CRoiDlg::OnEnChangeXmax() {
  buttonApply.EnableWindow(true);
}


void
CRoiDlg::OnEnChangeYmin() {
  buttonApply.EnableWindow(true);
}


void
CRoiDlg::OnEnChangeYmax() {
  buttonApply.EnableWindow(true);
}


void
CRoiDlg::OnBnClickedApply() {
  UpdateData(true);
  if(m_XMin > m_XMax) {
    int tmp = m_XMax;
    m_XMax = m_XMin;
    m_XMin = tmp;
  }
  if(m_YMin > m_YMax) {
    int tmp = m_YMax;
    m_YMax = m_YMin;
    m_YMin = tmp;
  }
  myRect = CRect(m_XMin, m_YMin, m_XMax, m_YMax);
  if(pmyParent != NULL)
    pmyParent->PostMessage(IMAGE_SELECTION_CHANGED, 0, (LPARAM)&myRect);
  buttonApply.EnableWindow(false);
}
