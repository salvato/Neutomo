// MoveCenterDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "NeuTomo.h"
#include "MoveCenterDlg.h"


// finestra di dialogo CMoveCenterDlg

IMPLEMENT_DYNAMIC(CMoveCenterDlg, CDialog)

CMoveCenterDlg::CMoveCenterDlg(CWnd* pParent /*=NULL*/)
	: pMyParent(pParent)
  , CenterPos(0.0)
  , TiltAngle(0.0)
  , CDialog(CMoveCenterDlg::IDD, pParent)
{

}


CMoveCenterDlg::~CMoveCenterDlg()
{
}


void
CMoveCenterDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CENTER_POS, editCenterPos);
  DDX_Control(pDX, IDC_TILT_ANGLE, editTiltAngle);
}


BEGIN_MESSAGE_MAP(CMoveCenterDlg, CDialog)
  ON_BN_CLICKED(IDC_CCW,   &CMoveCenterDlg::OnBnClickedCCW)
  ON_BN_CLICKED(IDC_CW,    &CMoveCenterDlg::OnBnClickedCW)
  ON_BN_CLICKED(IDC_LEFT,  &CMoveCenterDlg::OnBnClickedLeft)
  ON_BN_CLICKED(IDC_RIGHT, &CMoveCenterDlg::OnBnClickedRight)
END_MESSAGE_MAP()


void 
CMoveCenterDlg::SetParent(CWnd* pParent){
  pMyParent = pParent;
}


// gestori di messaggi CMoveCenterDlg

void 
CMoveCenterDlg::OnBnClickedCCW() {
  pMyParent->PostMessage(MSG_ROTATE_CCW);
}


void
CMoveCenterDlg::OnBnClickedCW() {
  pMyParent->PostMessage(MSG_ROTATE_CW);
}


void
CMoveCenterDlg::OnBnClickedLeft() {
  pMyParent->PostMessage(MSG_MOVE_LEFT);
}


void
CMoveCenterDlg::OnBnClickedRight() {
  pMyParent->PostMessage(MSG_MOVE_RIGHT);
}


void
CMoveCenterDlg::SetCenterPos(float newPos) {
  CenterPos = newPos;
  sString.Format(_T("%.03f"), CenterPos);
  editCenterPos.SetWindowText(sString);
}


void
CMoveCenterDlg::SetTiltAngle(float newTilt) {
  TiltAngle = newTilt;
  sString.Format(_T("%.03f"), TiltAngle);
  editTiltAngle.SetWindowText(sString);
}


float
CMoveCenterDlg::GetCenterPos() {
  return CenterPos;
}


float
CMoveCenterDlg::GetTiltAngle() {
  return TiltAngle;
}


BOOL
CMoveCenterDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  sString.Format(_T("%.03f"), TiltAngle);
  editTiltAngle.SetWindowText(sString);
  sString.Format(_T("%.03f"), CenterPos);
  editCenterPos.SetWindowText(sString);

  return TRUE;  // return TRUE unless you set the focus to a control
  // ECCEZIONE: le pagine delle proprietà OCX devono restituire FALSE
}
