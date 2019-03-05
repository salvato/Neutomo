// PlayCntlDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "NeuTomo.h"
#include "PlayCntlDlg.h"


// finestra di dialogo CPlayCntlDlg

IMPLEMENT_DYNAMIC(CPlayCntlDlg, CDialog)

CPlayCntlDlg::CPlayCntlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayCntlDlg::IDD, pParent)
  , pMyParent(pParent)
  , xControl(0)
  , yControl(0)
{

}


CPlayCntlDlg::~CPlayCntlDlg() {
}


void
CPlayCntlDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PLAY, buttonPlay);
  DDX_Control(pDX, IDC_STOP, buttonStop);
  DDX_Control(pDX, IDC_PREV, buttonBack);
  DDX_Control(pDX, IDC_NEXT, buttonNext);
}


BEGIN_MESSAGE_MAP(CPlayCntlDlg, CDialog)
  ON_BN_CLICKED(IDC_PLAY, &CPlayCntlDlg::OnBnClickedPlay)
  ON_BN_CLICKED(IDC_STOP, &CPlayCntlDlg::OnBnClickedStop)
  ON_BN_CLICKED(IDC_PREV, &CPlayCntlDlg::OnBnClickedPrev)
  ON_BN_CLICKED(IDC_NEXT, &CPlayCntlDlg::OnBnClickedNext)
  ON_WM_CREATE()
  ON_WM_CLOSE()
END_MESSAGE_MAP()

/*
To include a bitmap-button control in a dialog box 
Create one to four bitmap images for the button.

Create a dialog template with an owner-draw button 
positioned where you want the bitmap button. 
The size of the button in the template does not matter.
Set the button's caption to a value such as "MYIMAGE" 
and define a symbol for the button such as IDC_MYIMAGE.
In your application's resource script, give each of the 
images created for the button an ID constructed 
by appending one of the letters "U," "D," "F," or "X" 
(for up, down, focused, and disabled) to the string 
used for the button caption in step 3. 
For the button caption "MYIMAGE," for example, the IDs 
would be "MYIMAGEU," "MYIMAGED," "MYIMAGEF," and "MYIMAGEX." 
You must specify the ID of your bitmaps within double quotes. 
Otherwise the resource editor will assign an integer to the 
resource and MFC will fail when loading the image.

In your application's dialog class (derived from CDialog), 
add a CBitmapButton member object.
In the CDialog object's OnInitDialog routine, call the 
CBitmapButton object's AutoLoad function, 
using as parameters the button's control ID and the 
CDialog object's this pointer.

If you want to handle Windows notification messages, 
such as BN_CLICKED, sent by a bitmap-button control 
to its parent (usually a class derived from CDialog), 
add to the CDialog-derived object a message-map entry 
and message-handler member function for each message. 
The notifications sent by a CBitmapButton object are 
the same as those sent by a CButton object.
*/


// gestori di messaggi CPlayCntlDlg


BOOL
CPlayCntlDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  buttonStop.EnableWindow(false);

  CRect Rect;
  GetWindowRect(&Rect);
  Rect.SetRect(xControl, yControl, xControl+Rect.Width(), yControl+Rect.Height());
  MoveWindow(&Rect, true);

  return TRUE;  // return TRUE unless you set the focus to a control
  // ECCEZIONE: le pagine delle proprietà OCX devono restituire FALSE
}


void 
CPlayCntlDlg::OnBnClickedPlay() {
  buttonPlay.EnableWindow(false);
  buttonStop.EnableWindow(true);
  buttonBack.EnableWindow(false);
  buttonNext.EnableWindow(false);
  if(pMyParent != NULL) 
    pMyParent->SendMessage(MSG_PLAY);
}


void
CPlayCntlDlg::OnBnClickedStop() {
  buttonPlay.EnableWindow(true);
  buttonStop.EnableWindow(false);
  buttonBack.EnableWindow(true);
  buttonNext.EnableWindow(true);
  if(pMyParent != NULL) 
    pMyParent->SendMessage(MSG_STOP);
}


void
CPlayCntlDlg::OnBnClickedPrev() {
  if(pMyParent != NULL) 
    pMyParent->SendMessage(MSG_PREV);
}


void
CPlayCntlDlg::OnBnClickedNext() {
  if(pMyParent != NULL) 
    pMyParent->SendMessage(MSG_NEXT);
}


int
CPlayCntlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  int xx0, yy0;
  xx0 = AfxGetApp()->GetProfileInt(_T("PlayCntlDlg"), _T("MainWnd X0"), 0);
  yy0 = AfxGetApp()->GetProfileInt(_T("PlayCntlDlg"), _T("MainWnd Y0"), 0);
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


void
CPlayCntlDlg::OnClose() {
  CRect Rect;
  GetWindowRect(Rect);
  AfxGetApp()->WriteProfileInt(_T("PlayCntlDlg"), _T("MainWnd X0"), Rect.TopLeft().x);
  AfxGetApp()->WriteProfileInt(_T("PlayCntlDlg"), _T("MainWnd Y0"), Rect.TopLeft().y);
  pMyParent->PostMessage(MSG_CLOSE_PLAY, WPARAM(0));
}
