// CenterDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "NeuTomo.h"
#include "CenterDlg.h"
#include "afxdialogex.h"


// finestra di dialogo CCenterDlg

IMPLEMENT_DYNAMIC(CCenterDlg, CDialogEx)

CCenterDlg::CCenterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCenterDlg::IDD, pParent)
  , pmyParent(pParent)
  , xControl(0)
  , yControl(0)
{
  Create(IDD, pmyParent);
}


CCenterDlg::~CCenterDlg() {
  SaveDlgPos();
}


void 
CCenterDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CENTER_POS, editCenter);
  DDX_Control(pDX, IDC_TILT_ANGLE, editTilt);
}


void
CCenterDlg::SaveDlgPos() {
  if(!IsIconic()) {
    CRect Rect;
    GetWindowRect(Rect);
    AfxGetApp()->WriteProfileInt(__KEY__, _T("CenterWnd X0"), Rect.TopLeft().x);
    AfxGetApp()->WriteProfileInt(__KEY__, _T("CenterWnd Y0"), Rect.TopLeft().y);
  }
}


void
CCenterDlg::ReadDlgPos() {
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  int xx0 = AfxGetApp()->GetProfileInt(__KEY__, _T("CenterWnd X0"), xControl);
  int yy0 = AfxGetApp()->GetProfileInt(__KEY__, _T("CenterWnd Y0"), yControl);
  if((xx0>0) && (yy0>0) && (xx0<rDeskTop.right-20) && (yy0<rDeskTop.bottom-20)) {
    xControl = xx0;
    yControl = yy0;
  } else {
    xControl = 0;
    yControl = 0;
  }
}


void
CCenterDlg::SetCenter(double center) {
  Center = center;
}


void
CCenterDlg::SetTilt(double tilt) {
  Tilt = tilt;
}


double
CCenterDlg::GetCenter() {
  return Center;
}


double
CCenterDlg::GetTilt() {
  return Tilt;
}


BEGIN_MESSAGE_MAP(CCenterDlg, CDialogEx)
  ON_EN_CHANGE(IDC_CENTER_POS, &CCenterDlg::OnEnChangeCenterPos)
  ON_EN_CHANGE(IDC_TILT_ANGLE, &CCenterDlg::OnEnChangeTiltAngle)
  ON_BN_CLICKED(IDOK, &CCenterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// gestori di messaggi CCenterDlg


BOOL 
CCenterDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();
  ReadDlgPos();
  CRect Rect;
  GetWindowRect(&Rect);
  Rect.SetRect(xControl, yControl, xControl+Rect.Width(), yControl+Rect.Height());
  MoveWindow(&Rect, true);

  return TRUE; 
}


void
CCenterDlg::OnEnChangeCenterPos() {
  editCenter.GetWindowText(sString);
  Center = _ttof(sString);
  //if(pmyParent) {
  //  pmyParent->PostMessage();
  //}
}


void 
CCenterDlg::OnEnChangeTiltAngle() {
  editTilt.GetWindowText(sString);
  Tilt = _ttof(sString);
  //if(pmyParent) {
  //  pmyParent->PostMessage();
  //}
}


void CCenterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
