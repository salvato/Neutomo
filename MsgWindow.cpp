/////////////////////////////////////////////////////////////////////////////
// MsgWindow.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MsgWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgWindow
/////////////////////////////////////////////////////////////////////////////

CMsgWindow::CMsgWindow(CString Title/*="Messges"*/) {
  pMessages = NULL;

  UINT cStyle= CS_HREDRAW | CS_NOCLOSE | CS_OWNDC | CS_VREDRAW;
  HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICONMSG);
  CString ClassName;
  ClassName = AfxRegisterWndClass(cStyle, 0, 0, hIcon);
  DWORD exStyle = WS_EX_DLGMODALFRAME;
  DWORD Style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
  BOOL result = CreateEx(exStyle, ClassName, Title, Style, 0, 40, 656, 480, NULL, NULL);
  pmyFont = NULL;
}


CMsgWindow::~CMsgWindow() {
  if(!IsIconic()) {
    CWinApp* pApp = AfxGetApp();
    CString strValue;
    CString strSection = _T("Messages");
    CString strStringItem;
    GetWindowText(strStringItem);
    CRect Rect;
    GetWindowRect(&Rect);
    strValue.Format(_T("%d,%d,%d,%d"), Rect.left, Rect.top, Rect.Width(), Rect.Height());  
    pApp->WriteProfileString(strSection, strStringItem, strValue);
  }
  if(pMessages!=NULL) {
    delete pMessages->GetFont();
    delete pMessages;
  }
}


BEGIN_MESSAGE_MAP(CMsgWindow, CWnd)
	//{{AFX_MSG_MAP(CMsgWindow)
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMsgWindow message handlers
/////////////////////////////////////////////////////////////////////////////

void 
CMsgWindow::AddText(CString Text) {
  long nChars = pMessages->GetWindowTextLength();
  if(nChars > 19000) {
    pMessages->SetSel(0, 9000);
    pMessages->ReplaceSel(_T("<Too Many Characters: Cut>\r\n"));
    nChars = pMessages->GetWindowTextLength();
  }
  if(nChars != 0) {
    pMessages->SetSel(nChars, nChars);
    pMessages->ReplaceSel(Text);
  } else
    pMessages->SetWindowText(Text);
}

int 
CMsgWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  int x0   = 0;
  int y0   = 0;
  int wdth = 560;
  int hght = 425;

  CWinApp* pApp = AfxGetApp();
  CString strValue;
  CString strSection = _T("Messages");
  CString strStringItem;
  strStringItem = lpCreateStruct->lpszName;
  strValue = pApp->GetProfileString(strSection, strStringItem);
  if(strValue != "") {
    int curPos = 0;
    x0   = _wtoi(strValue.Tokenize(_T(","), curPos));
    y0   = _wtoi(strValue.Tokenize(_T(","), curPos));
    wdth = _wtoi(strValue.Tokenize(_T(","), curPos));
    hght = _wtoi(strValue.Tokenize(_T("\0"), curPos));
  }
  CRect Rect;
  GetWindowRect(&Rect);
  Rect.SetRect(x0, y0, x0+wdth, y0+hght);
  MoveWindow(&Rect, true);	
	
  pMessages = new CEdit();
  DWORD Style = WS_CHILD|WS_VISIBLE|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY;
  Style |= WS_VSCROLL;
  GetClientRect(&Rect);
  if(!pMessages->Create(Style, Rect, this, 1)) {
    return -1;
  }
  pmyFont = new CFont();
  pmyFont->CreateFont(14, 0, 0, 0, FW_DONTCARE, false, false, 0, ANSI_CHARSET, 
                      OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                      FIXED_PITCH, _T("Arial"));
  pMessages->SetFont(pmyFont);

	CMenu* pSysMenu= GetSystemMenu(false);
	pSysMenu->AppendMenu(MF_SEPARATOR);
	pSysMenu->AppendMenu(MF_STRING, IDM_CLEARTEXT, _T("Clear Text"));
	return 0;
}

HBRUSH 
CMsgWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	switch (nCtlColor) {
	  case CTLCOLOR_STATIC:
		  pDC->SetTextColor(RGB(255, 255, 255));
		  pDC->SetBkColor(RGB(0, 0, 0));
		  return (HBRUSH)GetStockObject(BLACK_BRUSH);
	  default:
	    HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	    return hbr;
  }
}


void 
CMsgWindow::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);
  if(pMessages != NULL) {
    CRect Rect;
    GetClientRect(&Rect);
    pMessages->MoveWindow(&Rect, true);
  }
}

void 
CMsgWindow::OnSysCommand(UINT nID, LPARAM lParam) {
  if(nID == IDM_CLEARTEXT) {
    pMessages->SetSel(0, -1);
    pMessages->SetWindowText(_T(""));
  } else {
		CWnd::OnSysCommand(nID, lParam);
	}
}


void
CMsgWindow::ClearText() {
  pMessages->SetSel(0, -1);
  pMessages->SetWindowText(_T(""));
}
