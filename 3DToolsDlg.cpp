// 3DToolsDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "resource.h"
#include "3DToolsDlg.h"


// finestra di dialogo C3DToolsDlg

IMPLEMENT_DYNAMIC(C3DToolsDlg, CDialog)

C3DToolsDlg::C3DToolsDlg(CWnd* pParent /*=NULL*/)
  : pMyParent(pParent)
  , xControl(0)
  , yControl(0)
  , CDialog(C3DToolsDlg::IDD, pParent) 
{
}


C3DToolsDlg::~C3DToolsDlg() {
}


int
C3DToolsDlg::GetIntensity() {
  return(sliderIntensity.GetPos());
}


void
C3DToolsDlg::SetIntensity(int Intensity) {
  sliderIntensity.SetPos(Intensity);
}


int
C3DToolsDlg::GetRed() {
  return(sliderRed.GetPos());
}


void
C3DToolsDlg::SetRed(int Red) {
  sliderRed.SetPos(Red);
}


int
C3DToolsDlg::GetGreen() {
  return(sliderGreen.GetPos());
}


void
C3DToolsDlg::SetGreen(int Green) {
  sliderGreen.SetPos(Green);
}


int
C3DToolsDlg::GetBlue() {
  return(sliderBlue.GetPos());
}


void
C3DToolsDlg::SetBlue(int Blue) {
  sliderBlue.SetPos(Blue);
}


void
C3DToolsDlg::GetIntensityRange(int& min, int& max){
  sliderIntensity.GetRange(min, max);
}


void
C3DToolsDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_INTENSITY, sliderIntensity);
  DDX_Control(pDX, IDC_RED, sliderRed);
  DDX_Control(pDX, IDC_GREEN, sliderGreen);
  DDX_Control(pDX, IDC_BLUE, sliderBlue);
  DDX_Control(pDX, IDC_SHOW_FRAME, buttonShowFrame);
}


BEGIN_MESSAGE_MAP(C3DToolsDlg, CDialog)
  ON_WM_VSCROLL()
  ON_WM_CREATE()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_SHOW_FRAME, &C3DToolsDlg::OnBnClickedShowFrame)
END_MESSAGE_MAP()


// gestori di messaggi C3DToolsDlg


void 
C3DToolsDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  CSliderCtrl* pSource;
  pSource = (CSliderCtrl *)pScrollBar;
  
  switch(nSBCode) {
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      if(pSource == &sliderRed) {
        pMyParent->SendMessage(MSG_CHANGE_RED, (WPARAM)nPos);
      } else if(pSource == &sliderGreen) {
        pMyParent->SendMessage(MSG_CHANGE_GREEN, (WPARAM)nPos);
      } else if(pSource == &sliderBlue) {
        pMyParent->SendMessage(MSG_CHANGE_BLUE, (WPARAM)nPos);
      } else if(pSource == &sliderIntensity) { 
        pMyParent->SendMessage(MSG_CHANGE_ISO, (WPARAM)nPos);
      }
      break;

    case SB_LINEDOWN:
    case SB_LINEUP:
    case SB_PAGEDOWN:
    case SB_PAGEUP:
      if(pSource == &sliderRed) {
        pMyParent->SendMessage(MSG_CHANGE_RED, (WPARAM)(pSource->GetPos()));
      } else if(pSource == &sliderGreen) {
        pMyParent->SendMessage(MSG_CHANGE_GREEN, (WPARAM)(pSource->GetPos()));
      } else if(pSource == &sliderBlue) {
        pMyParent->SendMessage(MSG_CHANGE_BLUE, (WPARAM)(pSource->GetPos()));
      } else if(pSource == &sliderIntensity) { 
        pMyParent->SendMessage(MSG_CHANGE_ISO, (WPARAM)(pSource->GetPos()));
      }
      break;
  }

  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


int
C3DToolsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  int xx0, yy0;
  xx0 = AfxGetApp()->GetProfileInt(_T("3DToolsDlg"), _T("MainWnd X0"), 0);
  yy0 = AfxGetApp()->GetProfileInt(_T("3DToolsDlg"), _T("MainWnd Y0"), 0);
  if((xx0>0) && (yy0>0) && (xx0<rDeskTop.right-20) && (yy0<rDeskTop.bottom-20)) {
    xControl = xx0;
    yControl = yy0;
  } else {
    xControl = 0;
    yControl = 0;
  }
  HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	this->SetIcon(hIcon, FALSE);

  return 0;
}


BOOL
C3DToolsDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  sliderIntensity.SetRange(0, 1000);
  sliderRed.SetRange(0, 100);
  sliderGreen.SetRange(0, 100);
  sliderBlue.SetRange(0, 100);
  SetIntensity(3);
  SetRed(100);
  SetGreen(100);
  SetBlue(30);

  CRect Rect;
  GetWindowRect(&Rect);
  Rect.SetRect(xControl, yControl, xControl+Rect.Width(), yControl+Rect.Height());
  MoveWindow(&Rect, true);

  return TRUE;  // return TRUE unless you set the focus to a control
}


void
C3DToolsDlg::OnBnClickedShowFrame() {
  if(buttonShowFrame.GetCheck() == BST_CHECKED) {
    pMyParent->SendMessage(MSG_SHOW_FRAME, WPARAM(1));
  } else {
    pMyParent->SendMessage(MSG_SHOW_FRAME, WPARAM(0));
  }
}


int
C3DToolsDlg::GetShowFrame() {
  if(buttonShowFrame.GetCheck() == BST_CHECKED) {
     return 1;
  } else {
    return 0;
  }
}


void
C3DToolsDlg::OnClose() {
  CRect Rect;
  GetWindowRect(Rect);
  AfxGetApp()->WriteProfileInt(_T("3DToolsDlg"), _T("MainWnd X0"), Rect.TopLeft().x);
  AfxGetApp()->WriteProfileInt(_T("3DToolsDlg"), _T("MainWnd Y0"), Rect.TopLeft().y);
  pMyParent->PostMessage(MSG_CLOSE_VOLUME, WPARAM(0));
}

