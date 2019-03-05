#include "StdAfx.h"
#include "resource.h"

#include <math.h>

#include ".\AlignWnd.h"
#include "Projection.h"


/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265
#endif


CAlignWnd::CAlignWnd(CWnd* pWndParent, CString sTitle) 
  : pParent(pWndParent)
  , sMyTitle(sTitle)
  , msgEndAlignment(WM_USER+1000)
  , msgAlignmentChanged(WM_USER+1001)
  , msgAbortAlignment(WM_USER+1002)
  , fStep(1.0)
  , xControl(100)
  , yControl(100)
  , winWidth(300)
  , winHeight(300)
{
  ReadWndPos();
  UINT cStyle= CS_DBLCLKS | CS_HREDRAW | CS_NOCLOSE | CS_VREDRAW;
  CBrush myBrush;
  myBrush.CreateSolidBrush(RGB(255, 255, 255));
  HBRUSH hbrBackground = HBRUSH(myBrush);
  HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  CString ClassName = AfxRegisterWndClass(cStyle, NULL, hbrBackground, hIcon); 
  DWORD exStyle = WS_EX_DLGMODALFRAME;
  DWORD Style = WS_OVERLAPPEDWINDOW;
  BOOL result = CreateEx(exStyle, ClassName, _T("Aligning Window"), Style, 100, 100, winWidth, winHeight, NULL, NULL, 0);
  if(!result) {
    AfxMessageBox(_T("CAlignWnd CreateEx(...) failed"));
    PostQuitMessage(-1);
  } // if(!result)

}


CAlignWnd::~CAlignWnd(void) {
  SaveWndPos();
}


void
CAlignWnd::SetEndMessage(UINT Message) {
  msgEndAlignment = Message; 
}


void
CAlignWnd::SetAbortMessage(UINT Message) {
  msgAbortAlignment = Message; 
}


void
CAlignWnd::SetAlignmentChangedMessage(UINT Message) {
  msgAlignmentChanged = Message; 
}


void
CAlignWnd::SaveWndPos() {
  if(!IsIconic()) {
    CRect Rect;
    GetWindowRect(Rect);
    AfxGetApp()->WriteProfileInt(__KEY__, sMyTitle + _T(" X0"), Rect.TopLeft().x);
    AfxGetApp()->WriteProfileInt(__KEY__, sMyTitle + _T(" Y0"), Rect.TopLeft().y);
  }
}


void
CAlignWnd::ReadWndPos() {
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  int xx0 = AfxGetApp()->GetProfileInt(__KEY__, sMyTitle + _T(" X0"), xControl);
  int yy0 = AfxGetApp()->GetProfileInt(__KEY__, sMyTitle + _T(" Y0"), yControl);
  if((xx0>0) && (yy0>0) && (xx0<rDeskTop.right-20) && (yy0<rDeskTop.bottom-20)) {
    xControl = xx0;
    yControl = yy0;
  } else {
    xControl = 0;
    yControl = 0;
  }
}


bool
CAlignWnd::LoadTextures(float* pTex0, float* pTex180, int Nx, int Ny) {
  texWidth  = Nx;
  texHeight = Ny;

  BeginOpenGLDrawing();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  // Create the two Textures
  glGenTextures(2, inTexID);

  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, inTexID[0]);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S,     GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T,     GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_LUMINANCE32F_ARB, texWidth, texHeight, 0, GL_LUMINANCE, GL_FLOAT, pTex0);

  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, inTexID[1]);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S,     GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T,     GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_LUMINANCE32F_ARB, texWidth, texHeight, 0, GL_LUMINANCE, GL_FLOAT, pTex180);

  bool bError = CheckGLErrors(_T("LoadTextures"));
  EndOpenGLDrawing();
  return bError;
}


void
CAlignWnd::Initialize(int Width, int Height) {
  CRect myRect;
  GetWindowRect(&myRect);
  myRect.SetRect(myRect.left, myRect.top, myRect.left+Width, myRect.top+Height);
  int x0 = myRect.left;
  int y0 = myRect.top;
  CalcWindowRect(&myRect,  CWnd::adjustBorder);
  myRect.MoveToXY(x0, y0);
  MoveWindow(myRect);

  rot   = 0.0;
  tran  = 0.0;
}


void
CAlignWnd::CheckErrors() {
  GLenum error;
  CString sError;
  while((error = glGetError()) != GL_NO_ERROR) {
    sError.Format(_T("Error: %s\n"), (char *) gluErrorStringWIN(error));
    AfxMessageBox(sError);
  }
}

///////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAlignWnd, COpenGLCtrl)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_SIZE()
END_MESSAGE_MAP()


void 
CAlignWnd::PostNcDestroy() {
  delete this;
}


void 
CAlignWnd::OnPrepareRC() {
  int err = glewInit();
  if(err != GLEW_OK) {// sanity check
    char* cString = (char *)glewGetErrorString(err);
    _TCHAR* tString = new _TCHAR[strlen(cString)+1];
    mbstowcs(tString, cString, strlen(cString));
    tString[strlen(cString)] = 0;
    AfxMessageBox(tString, MB_ICONSTOP);
    delete[] tString;
  } // if(GLEW_OK != err)
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
}


void 
CAlignWnd::OnDestroyRC() {
}


void 
CAlignWnd::OnRender() {
  CRect myRect;
  GetWindowRect(&myRect);
  glViewport(0, 0, (GLsizei)myRect.Width(), (GLsizei)myRect.Height());

  glClearColor(0.5, 0.5, 0.5, 0.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, inTexID[0]);

  glEnable (GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);

  glColor4f(1.0, 1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0,      0.0);       glVertex2f(-1.0, -1.0);
    glTexCoord2f(0.0,      texHeight); glVertex2f(-1.0,  1.0);
    glTexCoord2f(texWidth, texHeight); glVertex2f( 1.0,  1.0);
    glTexCoord2f(texWidth, 0.0);       glVertex2f( 1.0, -1.0);
  glEnd();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, inTexID[1]);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

  glRotatef(2.0*rot, 0.0, 0.0, 1.0);
  glTranslatef(-2.0*tran, 0.0, 0.0);

  glColor4f(1.0, 1.0, 1.0, 0.5);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0,      0.0);       glVertex2f(-1.0, -1.0);
    glTexCoord2f(0.0,      texHeight); glVertex2f(-1.0,  1.0);
    glTexCoord2f(texWidth, texHeight); glVertex2f( 1.0,  1.0);
    glTexCoord2f(texWidth, 0.0);       glVertex2f( 1.0, -1.0);
  glEnd();

  glDisable (GL_BLEND);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);

  glColor3f(1.0, 1.0, 0.0);// Text Color
  CString sString;
  sString.Format(_T("Dx = %.3f Theta = %.3f°"), -(tran*texWidth)*0.5, -0.5*rot);
  glRasterPos2f(-0.95, 0.75);
  for(int i=0; i<sString.GetLength(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)sString.GetAt(i));
  }

  glFlush();
  glDisable(GL_TEXTURE_RECTANGLE_ARB);

  CheckErrors();
}


void 
CAlignWnd::OnViewing() {
}


void 
CAlignWnd::OnProjection(GLdouble AspectRatio) {
}


void 
CAlignWnd::OnViewport(int cx, int cy) {
}

void
CAlignWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if(nChar == VK_END) {// End key
    if(pParent) pParent->PostMessage(msgEndAlignment);
  } else if(nChar == VK_ESCAPE) {
    if(pParent) pParent->PostMessage(msgAbortAlignment);
  } else if(nChar == VK_RIGHT) {
    tran -= fStep*nRepCnt / float(texWidth);
    if(pParent) pParent->PostMessage(msgAlignmentChanged);
    RedrawOpenGLWindow();
  } else if(nChar == VK_LEFT) {
    tran += fStep*nRepCnt / float(texWidth);
    if(pParent) pParent->PostMessage(msgAlignmentChanged);
    RedrawOpenGLWindow();
  } else if(nChar == VK_UP) {
    rot += 10.0*fStep*nRepCnt / float(texWidth);
    if(pParent) pParent->PostMessage(msgAlignmentChanged);
    RedrawOpenGLWindow();
  } else if(nChar == VK_DOWN) {
    rot -= 10.0*fStep*nRepCnt / float(texWidth);
    if(pParent) pParent->PostMessage(msgAlignmentChanged);
    RedrawOpenGLWindow();
  } else if(nChar == VK_SHIFT) {
    fStep = 10.0;
  }

  COpenGLCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void
CAlignWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if(nChar == VK_SHIFT) {
    fStep = 1.0;
  }

  COpenGLCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void
CAlignWnd::OnMouseMove(UINT nFlags, CPoint point) {
  float dx = point.x - startPoint.x;
  if(nFlags & MK_SHIFT) {
    if(nFlags & MK_LBUTTON) {
      tran -= 0.005*dx;
    } else if(nFlags & MK_RBUTTON) {
      rot += 0.1*dx;
    }
  } else {
    if(nFlags & MK_LBUTTON) {
      tran -= 0.00005*dx;
    } else if(nFlags & MK_RBUTTON) {
      rot += 0.002*dx;
    }
  }
  startPoint = point;
  RedrawOpenGLWindow();
}


void
CAlignWnd::OnLButtonDown(UINT nFlags, CPoint point) {
  startPoint = point;
}



void
CAlignWnd::OnRButtonDown(UINT nFlags, CPoint point) {
  startPoint = point;
}


void 
CAlignWnd::OnSize(UINT nType, int cx, int cy) {
  glViewport(0, 0, (GLsizei)cx, (GLsizei)cx);
}


void
CAlignWnd::SetTilt(float tilt) {
  rot = tilt;
}


void
CAlignWnd::SetPos(float pos) {
  tran = 2.0*pos / float(texWidth);
}


float
CAlignWnd::GetTilt(void) {
  return rot;
}


float
CAlignWnd::GetPos(void) {
  return tran * 0.5 * float(texWidth);
}
