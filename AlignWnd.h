#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include "oglctrl.h"


class 
CAlignWnd  :
  public COpenGLCtrls {

public:
  CAlignWnd(CWnd* pWndParent=NULL, CString sTitle=_T("Aligning Window"));
  virtual ~CAlignWnd(void);

protected:
  virtual void PostNcDestroy();
  void OnPrepareRC();
  void OnDestroyRC();
  void OnRender();
  void OnViewing();
  void OnProjection(GLdouble AspectRatio);
  void OnViewport(int cx, int cy);
  
  void CheckErrors();
  GLuint inTexID[2];

  int  winWidth, winHeight;
  int  texWidth, texHeight;
  float rot, tran, fStep;
  CPoint startPoint;
  CWnd* pParent;
  UINT msgEndAlignment;
  UINT msgAbortAlignment;
  UINT msgAlignmentChanged;
  CString sMyTitle;

public:
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  DECLARE_MESSAGE_MAP()

public:
  void SetEndMessage(UINT Message);
  void SetAlignmentChangedMessage(UINT Message);
  void SetAbortMessage(UINT Message);
  void Initialize(int Width, int Height);
  bool LoadTextures(float* pTex0, float* pTex180, int Nx, int Ny);
  void SetTilt(float tilt);
  void SetPos(float pos);
  float GetTilt(void);
  float GetPos(void);

private:
  void SaveWndPos();
  void ReadWndPos();
  int xControl, yControl;
};
