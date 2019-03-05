#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include "GrCamera.h"
#include "oglctrl.h"

class CShowVolWnd :
  public COpenGLCtrl
{
public:
  CShowVolWnd(CString Title=_T("OpenGL Window"), int xSize=640, int ySize=480, CWnd* pParent=NULL);
  virtual ~CShowVolWnd(void);

  void  SetDensity(float newDensity);
  void  SetIntensity(float newIntensity);
  float GetDensity();
  float GetIntensity();
  void  SetSliceN(float nSlices);
  int   GetSliceN();
  void  SetShownTexture(int Texture);
  int   GetShownTexture();
  void  SetShowFrame(int ShowFrame);

  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg LRESULT OnLoadVolume(WPARAM wParam, LPARAM lParam);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

protected:
  void OnPrepareRC();
  void OnDestroyRC();
  void OnRender();
  void OnViewing();
  void OnProjection(GLdouble AspectRatio);
  void OnViewport(int cx, int cy);
  void CustomizePixelFormat(PIXELFORMATDESCRIPTOR* pfd);
  bool CopyPix(CProjection& ProjTemp, CxImage* pImage, float fMin, float fMax);
  int  Init();

protected:
  CWnd *pMyParent;
  CString myTitle;
  int x0, y0, wdth, hght;
  int nProjections;
	CGrCamera m_camera;
	void ActualDraw();
  GLuint volTextures[2];
  float points[64][3];
  float xPlane[4], yPlane[4], zPlane[4];
  double clip0[4], clip1[4], clip2[4], clip3[4], clip4[4], clip5[4];
  int slices;
  void MapColor(GLubyte *color, float t);
  float plane[12];
  float mat[16];
  float objectXform[16], planeXform[16];
  float intensity;
  float density; 
  const int MAX_3D_TEXTURESIZE;
  int TextureShown;
  bool bShowFrame;
  bool bVolumeLoaded;

  DECLARE_MESSAGE_MAP()
};
