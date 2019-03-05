// SetIntervalDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "resource.h"
#include "SetIntervalDlg.h"


// finestra di dialogo CSetIntervalDlg

IMPLEMENT_DYNAMIC(CSetIntervalDlg, CDialog)

CSetIntervalDlg::CSetIntervalDlg(CWnd* pParent /*=NULL*/)
	: pmyParent(pParent), CDialog(CSetIntervalDlg::IDD, pParent)
{

}


CSetIntervalDlg::~CSetIntervalDlg() {
}


void
CSetIntervalDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_XMIN, editXMin);
  DDX_Control(pDX, IDC_XMAX, editXMax);
  DDX_Control(pDX, IDC_CHECK, buttonCheck);
  DDX_Text(pDX, IDC_XMIN, XMin);
  DDX_Text(pDX, IDC_XMAX, XMax);
}


BEGIN_MESSAGE_MAP(CSetIntervalDlg, CDialog)
  ON_BN_CLICKED(IDOK,      OnBnClickedAccept)
  ON_BN_CLICKED(IDC_CHECK, OnBnClickedCheck)
  ON_BN_CLICKED(IDCANCEL,  OnBnClickedCancel)
  ON_EN_CHANGE(IDC_XMIN,   OnEnChangeXMin)
  ON_EN_CHANGE(IDC_XMAX,   OnEnChangeXMax)
END_MESSAGE_MAP()


// gestori di messaggi CSetIntervalDlg

void
CSetIntervalDlg::OnBnClickedAccept() {
  UpdateData(true);
  if(XMin > XMax) {
    int tmp = XMax;
    XMax = XMin;
    XMin = tmp;
  }
  myPoint = CPoint(XMin, XMax);
  if(pmyParent != NULL)
    pmyParent->SendMessage(INTERVAL_CHANGED, 0, (LPARAM)&myPoint);
  OnOK();
}


void
CSetIntervalDlg::OnBnClickedCheck() {
  UpdateData(true);
  myPoint = CPoint(XMin, XMax);
  if(pmyParent != NULL)
    pmyParent->PostMessage(INTERVAL_CHANGED, 0, (LPARAM)&myPoint);
  buttonCheck.EnableWindow(false);
}


void
CSetIntervalDlg::OnBnClickedCancel() {
  OnCancel();
}


void
CSetIntervalDlg::OnEnChangeXMin() {
  buttonCheck.EnableWindow(true);
}


void 
CSetIntervalDlg::OnEnChangeXMax() {
  buttonCheck.EnableWindow(true);
}


BOOL
CSetIntervalDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  return TRUE;  // return TRUE unless you set the focus to a control
  // ECCEZIONE: le pagine delle proprietà OCX devono restituire FALSE
}
