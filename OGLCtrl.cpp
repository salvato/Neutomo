// File OGLCtrl.cpp

#include "stdafx.h"
#include ".\oglctrl.h"


COpenGLCtrl::COpenGLCtrl()
  : m_hRC(NULL)
  , m_hDC(NULL) 
{
}


BEGIN_MESSAGE_MAP(COpenGLCtrl, CWnd)
  ON_WM_PAINT()
  ON_WM_SIZE()
END_MESSAGE_MAP()


bool
COpenGLCtrl::CheckGLErrors(CString sModule) {
  CString sError, sNum;
  GLenum error;
  int iCount = 0;
  if((error = glGetError()) == GL_NO_ERROR) return true;
  sError = _T("Error in ") + sModule + _T("\r\n");
  while(((error = glGetError()) != GL_NO_ERROR) &&
        (++iCount < 10))
  {
    sNum.Format(_T("%2d) "), iCount);
    sError.Format(_T("%s %s \r\n"),sError + sNum, (char *)gluErrorStringWIN(error));
  } 
  AfxMessageBox(sError);
  return false;
}


// Checks framebuffer status.
// Copied directly out of the spec, modified to deliver a return value.
bool
COpenGLCtrl::CheckFramebufferStatus() {
  GLenum status;
  status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      return true;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, incomplete attachment\n"));
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	    AfxMessageBox(_T("Unsupported framebuffer format\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, missing attachment\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, attached images must have same dimensions\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, attached images must have same format\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, missing draw buffer\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, missing read buffer\n"));
      break;
  }
  return false;
}


BOOL
COpenGLCtrl::PreCreateWindow(CREATESTRUCT& cs) {
	// Added for OpenGL
  cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
  return CWnd::PreCreateWindow(cs);
}


void
COpenGLCtrl::PreSubclassWindow() {
   ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
   ASSERT((GetClassLong(m_hWnd, GCL_STYLE) & CS_PARENTDC) == 0);
   InitializeOpenGLWindow();

   CRect Rect;
   GetClientRect(&Rect);

   // If we are subclassing an existing window 
   // we might have missed the WM_SIZE message
   if (!Rect.IsRectEmpty())
    ResizeOpenGLWindow(Rect.Width(),Rect.Height());

   CWnd::PreSubclassWindow();
}


void
COpenGLCtrl::OnSize(UINT nType, int cx, int cy) {
  CWnd::OnSize(nType, cx, cy);
  if (cx>0 && cy>0)
     ResizeOpenGLWindow(cx,cy);
}


void
COpenGLCtrl::OnPaint() {
  CPaintDC DC(this);
  RedrawOpenGLWindow();
}   


BOOL
COpenGLCtrl::OnEraseBkgnd(CDC* pDC) {
  return TRUE; // Don't clear background, OpenGL will
}


void
COpenGLCtrl::OnDestroy() {
  UninitializeOpenGLWindow();
  CWnd::OnDestroy();
}


void
COpenGLCtrl::SetupPixelFormat(HDC hDC) {
  PIXELFORMATDESCRIPTOR pfd = 
  {
    sizeof(PIXELFORMATDESCRIPTOR),// size of this pfd
    1,                            // version number
    PFD_DRAW_TO_WINDOW            // support window
    | PFD_SUPPORT_OPENGL          // support OpenGL
    | PFD_DOUBLEBUFFER            // double buffered 
    , PFD_TYPE_RGBA,              // RGBA type
    24,                           // 24-bit color depth
    0, 0, 0, 0, 0, 0,             // color bits ignored
    0,                            // no alpha buffer
    0,                            // shift bit ignored
    0,                            // no accumulation buffer
    0, 0, 0, 0,                   // accum bits ignored
    32,                           // 32-bit z-buffer
    0,                            // no stencil buffer
    0,                            // no auxiliary buffer
    PFD_MAIN_PLANE,               // main layer
    0,                            // reserved
    0, 0, 0                       // layer masks ignored
  };

  // Let user change the pfd structure
  CustomizePixelFormat(&pfd);

  // Set the pixel format
  int pixelformat = ChoosePixelFormat(hDC, &pfd);
  SetPixelFormat(hDC, pixelformat, &pfd);
}


void
COpenGLCtrl::InitializeOpenGLWindow() {
  // Get the DC and set its pixel format
  m_hDC = ::GetDC(m_hWnd);
  SetupPixelFormat(m_hDC);

  // Create the Rendering Context and make it current
  m_hRC = wglCreateContext(m_hDC);
  wglMakeCurrent(m_hDC, m_hRC);

  // Let user do his one-time initialization
  OnPrepareRC();
}


void
COpenGLCtrl::OnPrepareRC() {
}


void
COpenGLCtrl::UninitializeOpenGLWindow() {
  // Delete the RC
  if(m_hRC) {   
    OnDestroyRC(); // Let derived classes clean up
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(m_hRC);
    m_hRC = NULL;
  }
  // Release the DC
  if(m_hDC) {
    ::ReleaseDC(m_hWnd, m_hDC);
    m_hDC=NULL;
  }
}


void
COpenGLCtrl::ResizeOpenGLWindow(int cx, int cy) {
  // Let user set his viewport transformation
  OnViewport(cx, cy);
  // Select Projection matrix and initialize it
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Get the aspect ratio and set perspective transform
  GLdouble AspectRatio=(GLdouble)cx/(GLdouble)cy;
  OnProjection(AspectRatio);
  // Select ModelView matrix and initialize it
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 
  // Let user set his viewing transformation
  OnViewing();
}


void
COpenGLCtrl::RedrawOpenGLWindow() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  glPushMatrix();
  OnRender(); // Draw your scene
  glPopMatrix();
  glFinish();
  SwapBuffers(m_hDC);
}


void
COpenGLCtrl::CustomizePixelFormat(PIXELFORMATDESCRIPTOR * pfd) {
}


void 
COpenGLCtrl::OnDestroyRC() {
}


void 
COpenGLCtrl::OnRender() {
}


void 
COpenGLCtrl::OnViewing() {
}


void 
COpenGLCtrl::OnProjection(GLdouble AspectRatio) {
}


void 
COpenGLCtrl::OnViewport(int cx, int cy) {
}


//==================================================


COpenGLCtrls::COpenGLCtrls() {
}


void
COpenGLCtrls::InitializeOpenGLWindow() {
  COpenGLCtrl::InitializeOpenGLWindow();
  wglMakeCurrent(NULL, NULL);
  ::ReleaseDC(m_hWnd, m_hDC);
  m_hDC = NULL;
}


void
COpenGLCtrls::UninitializeOpenGLWindow() {
  m_hDC = ::GetDC(m_hWnd);
  wglMakeCurrent(m_hDC, m_hRC);
  COpenGLCtrl::UninitializeOpenGLWindow();
}


void
COpenGLCtrls::BeginOpenGLDrawing() {
  m_hDC = ::GetDC(m_hWnd);
  wglMakeCurrent(m_hDC, m_hRC);
}


void
COpenGLCtrls::EndOpenGLDrawing() {
  wglMakeCurrent(NULL, NULL);
  ::ReleaseDC(m_hWnd, m_hDC);
  m_hDC=NULL; 
}


void
COpenGLCtrls::ResizeOpenGLWindow(int cx, int cy) {
  BeginOpenGLDrawing();
  COpenGLCtrl::ResizeOpenGLWindow(cx, cy);
  EndOpenGLDrawing();
}


void
COpenGLCtrls::RedrawOpenGLWindow() {
  BeginOpenGLDrawing();
  COpenGLCtrl::RedrawOpenGLWindow();
  EndOpenGLDrawing();
}
//End of File
