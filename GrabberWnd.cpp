////////////////////////////////////////////////////////
// GrabberWnd.cpp : file di implementazione
////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include ".\grabberwnd.h"

#define TIMER_TIP 1
#define TIME_TIP  3000

////////////////////////////////////////////////////////
// CGrabberWnd
////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CGrabberWnd, CWnd)
CGrabberWnd::CGrabberWnd(CString Title/*="Frame Gabber"*/, int xSize/*=640*/, int ySize/*=480*/, CWnd* pParent/*=NULL*/) 
  : x0(0)
  , y0(0)
  , wdth(xSize)
  , hght(ySize)
  , xRatio(1.0)
  , yRatio(1.0)
  , myParent(NULL)
  , Zooming(false)
  , bSelectionEnabled(false) 
{
  myParent = pParent;
  UINT cStyle= CS_DBLCLKS | CS_HREDRAW | CS_NOCLOSE | CS_OWNDC | CS_VREDRAW;
  CBrush myBrush;
  myBrush.CreateSolidBrush(RGB(0,0,0));
  HBRUSH hbrBackground = HBRUSH(myBrush);
  HICON hIcon = AfxGetApp()->LoadIcon(IDI_GRABBERICO);
  ClassName = AfxRegisterWndClass(cStyle, NULL, hbrBackground, hIcon); 
  DWORD exStyle = WS_EX_DLGMODALFRAME;
  DWORD Style = WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
  BOOL result = CreateEx(exStyle, ClassName, Title, Style, x0, y0, wdth, hght, NULL, NULL);
  if(!result) {
    AfxMessageBox(_T("CGrabberWnd CreateEx(...) failed"));
  }
}


CGrabberWnd::~CGrabberWnd() {
  if(!IsIconic() && !IsZoomed()) {
    CWinApp* pApp = AfxGetApp();
    strSection = "Grabber Window";
    strStringItem = "Position";
    CRect Rect;
    GetWindowRect(&Rect);
    strValue.Format(_T("%d,%d"), Rect.left, Rect.top);  
    pApp->WriteProfileString(strSection, strStringItem, strValue);
  }
}


BEGIN_MESSAGE_MAP(CGrabberWnd, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_NCMOUSEMOVE()
  ON_WM_CAPTURECHANGED()
  ON_WM_SETCURSOR()
  ON_WM_TIMER()
END_MESSAGE_MAP()


int
CGrabberWnd::Save(CString sFileName) {
  if(!Image.IsValid()) {
    return -2;
  }
  if(Image.Save(sFileName, CXIMAGE_FORMAT_BMP)) {
    return 0;
  } else {
    return -1;
  }
}


int
CGrabberWnd::ClearImage(void) {
  if(Image.IsValid()) {
    Image.Clear();
  }
  return 0;
}

void*
CGrabberWnd::CreateImage(int xSize, int ySize, DWORD wBpp, DWORD imagetype/*=0*/) {
  void* pImage = Image.Create(xSize, ySize, wBpp, imagetype);
  if(pImage) {
    CRect rect;
    GetClientRect(&rect);
    xRatio = double(xSize)/double(rect.Width());
    yRatio = double(ySize)/double(rect.Height());
  }
  return pImage;
}


void
CGrabberWnd::EnableSelection(bool bEnable) {
  bSelectionEnabled = bEnable;
  Selection.left = Selection.right = 0;
  Selection.top = Selection.bottom = 0;
}


bool
CGrabberWnd::IsSelectionEnabled() {
  return bSelectionEnabled;
}


void
CGrabberWnd::UpdateNow(void) {
  RECT rClient;
  GetClientRect(&rClient);
	RedrawWindow(&rClient, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}


void
CGrabberWnd::SetSelection(CRect* pRect) {
  RECT rClient;
	GetClientRect(&rClient);
	RedrawWindow(&rClient, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
  CClientDC dc(this);
  CPen Pen(PS_SOLID, 1, RGB(255, 255, 0));
  dc.SelectObject(Pen);
  ScaledSelection = *pRect;
  Selection.left = LONG(double(ScaledSelection.left)/xRatio);
  Selection.right = LONG(double(ScaledSelection.right)/xRatio);
  Selection.top = LONG(double(ScaledSelection.top)/yRatio);
  Selection.bottom = LONG(double(ScaledSelection.bottom)/yRatio);
  dc.MoveTo(Selection.left,  Selection.top);
  dc.LineTo(Selection.right, Selection.top);
  dc.LineTo(Selection.right, Selection.bottom);
  dc.LineTo(Selection.left,  Selection.bottom);
  dc.LineTo(Selection.left,  Selection.top);
  DeleteObject(Pen);
	RedrawWindow(&rClient, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}


////////////////////////////////////////////////////////
// gestori di messaggi CGrabberWnd
////////////////////////////////////////////////////////


afx_msg void
CGrabberWnd::OnMouseMove(UINT nFlags, CPoint point) {
  if(point == LastPoint) return;
  LastPoint = point;
  RECT rClient;
  GetClientRect(&rClient);
  CClientDC dc(this);
  CTipInfo* pTipInfo = myTip.GetTipInfo(TipPointer);
  RGBQUAD Color = Image.GetPixelColor(point.x, rClient.bottom-point.y, false);
  sText.Format(_T("x=%d y=%d (R:%d G:%d B:%d)"), int(point.x*xRatio+0.5), int(point.y*yRatio+0.5), Color.rgbRed, Color.rgbGreen, Color.rgbBlue);
  pTipInfo->SetText(sText);
  if(Zooming) {
    Selection.right  = LONG(point.x);
    Selection.bottom = LONG(point.y);
  } // if(Zooming)
	RedrawWindow(&rClient, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	myTip.ShowTip(point);
  SetTimer(TIMER_TIP, TIME_TIP, NULL);
}


afx_msg void
CGrabberWnd::OnLButtonDown(UINT nFlags, CPoint point) {
  if(!bSelectionEnabled) return;
  Selection.left = Selection.right = LONG(point.x);
  Selection.top = Selection.bottom = LONG(point.y);
  myCursor = AfxGetApp()->LoadCursor(IDC_SELECTION);
  SetCursor(myCursor);
  Zooming = true;
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	myTip.ShowTip(point);
  SetTimer(TIMER_TIP, TIME_TIP, NULL);
}


afx_msg void
CGrabberWnd::OnLButtonUp(UINT nFlags, CPoint point) {
  if(Zooming) {
    Zooming = false;
    myCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
    SetCursor(myCursor);
    Selection.right = LONG(point.x);
    Selection.bottom = LONG(point.y);
    if((Selection.right == Selection.left) ||(Selection.top == Selection.bottom)) return;
    LONG tmp;
    if(Selection.right < Selection.left){
      tmp = Selection.right;
      Selection.right = Selection.left;
      Selection.right = tmp;
    }
    if(Selection.top < Selection.bottom){
      tmp = Selection.top;
      Selection.top = Selection.bottom;
      Selection.bottom = tmp;
    }
    if(myParent != NULL) {
      ScaledSelection.left   = LONG(double(Selection.left)*xRatio+0.5);
      ScaledSelection.right  = LONG(double(Selection.right)*xRatio+0.5);
      ScaledSelection.top    = LONG(double(Selection.top)*yRatio+0.5);
      ScaledSelection.bottom = LONG(double(Selection.bottom)*yRatio+0.5);
      myParent->PostMessage(IMAGE_SELECTION_DONE, WPARAM(0), LPARAM(&ScaledSelection));
    }
  }
}


afx_msg void
CGrabberWnd::OnRButtonDown(UINT nFlags, CPoint point) {
  CString FileName = _T("Image.bmp");
  if(Image.IsValid()) {
    CFileDialog FileDialog(FALSE, _T(".bmp"), FileName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("Bitmap Data File (*.bmp)|*.bmp|All Files (*.*)|*.*||"));
    FileDialog.m_ofn.lpstrTitle = _T("Save Bitmap File");
    FileDialog.m_ofn.lpstrInitialDir = _T(".");
    if(FileDialog.DoModal() != IDOK) {
      return;
    }
    FileName = FileDialog.GetPathName();
    Image.Save(FileName, CXIMAGE_FORMAT_BMP);
	}
}


int
CGrabberWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
	myCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
  SetCursor(myCursor);
  // Sets the Delay and create tip window
  myTip.CreateTipWnd(this);
  CRect Rect;
  Rect.SetRect(x0, y0, x0+wdth, y0+hght);
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  
  int xx0, yy0;

  strSection = "Grabber Window";
  strStringItem = "Position";
  strValue = AfxGetApp()->GetProfileString(strSection, strStringItem);
  try {
    if(strValue != "") {
      int curPos = 0;
      xx0 = _wtoi(strValue.Tokenize(_T(","), curPos));
      yy0 = _wtoi(strValue.Tokenize(_T("\0"), curPos));
      if((xx0>0) && (yy0>0) && (xx0<rDeskTop.right-100) && (yy0<rDeskTop.bottom-100)) {
        x0 = xx0;
        y0 = yy0;
      }
      strValue.Format(_T("%d,%d"), x0, y0);  
      AfxGetApp()->WriteProfileString(strSection, strStringItem, strValue);
    }
  } catch (...) {
  }

  CalcWindowRect(&Rect,  CWnd::adjustBorder);
  Rect.MoveToXY(x0, y0);
  MoveWindow(&Rect, true);
  GetClientRect(&Rect);
  CTipInfo info(Rect, _T("Coordinate"));
  TipPointer = myTip.AddTip(info);
  Selection.left = Selection.right = 0;
  Selection.top = Selection.bottom = 0;
  return 0;
}


void
CGrabberWnd::OnPaint() {
  CPaintDC dc(this); // device context for painting
  if(Image.IsValid()) {
    CRect Rect;
    GetClientRect(&Rect);
    Image.Draw(dc.GetSafeHdc(), Rect);
  }
  if(bSelectionEnabled) {
    CPen Pen(PS_SOLID, 1, RGB(255, 255, 0));
    dc.SelectObject(Pen);
    dc.MoveTo(Selection.left, Selection.top);
    dc.LineTo(Selection.right, Selection.top);
    dc.LineTo(Selection.right, Selection.bottom);
    dc.LineTo(Selection.left, Selection.bottom);
    dc.LineTo(Selection.left, Selection.top);
    DeleteObject(Pen);
  }
}


void
CGrabberWnd::OnNcMouseMove(UINT nHitTest, CPoint point) {
  CWnd::OnNcMouseMove(nHitTest, point);
}


void
CGrabberWnd::OnCaptureChanged(CWnd *pWnd) {
  CWnd::OnCaptureChanged(pWnd);
}


BOOL
CGrabberWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
  SetCursor(myCursor);
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


void
CGrabberWnd::OnTimer(UINT nIDEvent) {
  if(nIDEvent == TIMER_TIP) {
    myTip.HideTip();
  }
  CWnd::OnTimer(nIDEvent);
}
