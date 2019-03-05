// File OGLCtrl.h
#pragma once

#include "gl\glew.h"    // OpenGL Utility Library
#include "gl\glut.h"    // OpenGL Utility Library

// Class for single OpenGL window
class COpenGLCtrl : public CWnd
{
  protected:

    HGLRC m_hRC;  // Rendering Context
    HDC   m_hDC;  // Device Context

  public:  

    COpenGLCtrl(); 

    // Register "MyOpenGL" window class
    static BOOL RegisterOpenGLWindowClass();

  protected:

    bool CheckGLErrors(CString sModule);
    bool CheckFramebufferStatus();

    // Initialization and uninitialization 
    virtual void InitializeOpenGLWindow();
    virtual void UninitializeOpenGLWindow();
 
    // Set the Pixel format
    virtual void SetupPixelFormat(HDC hDC);
    virtual void CustomizePixelFormat(PIXELFORMATDESCRIPTOR * pfd);

    // Handle WM_SIZE and WM_PAINT
    virtual void RedrawOpenGLWindow();
    virtual void ResizeOpenGLWindow(int cx, int cy);
  
    // Protocol: Redefine these to render your scene
    virtual void OnPrepareRC();
    virtual void OnDestroyRC();
    virtual void OnViewport(int cx, int cy);
    virtual void OnProjection(GLdouble AspectRatio);
    virtual void OnViewing();
    virtual void OnRender(); 

 protected:
    
    // Overridden MFC virtual functions
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void PreSubclassWindow();

    // MFC message handlers
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    //.....
public:
  DECLARE_MESSAGE_MAP()
};

// Class for many OpenGL windows
class COpenGLCtrls : public COpenGLCtrl
{
  public:

    COpenGLCtrls();      

  protected:

    virtual void InitializeOpenGLWindow();
    virtual void UninitializeOpenGLWindow();

    virtual void RedrawOpenGLWindow();
    virtual void ResizeOpenGLWindow(int cx, int cy);


    // Use these to call OpenGL functions outside protocol
    virtual void BeginOpenGLDrawing();
    virtual void EndOpenGLDrawing();
};

// Function that checks OpenGL errors
BOOL GetOpenGLError();
/* End of File */

