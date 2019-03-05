#pragma once
#include <vector>
#include "oglctrl.h"
#include "GrCamera.h"

using std::vector;

class CVolumeWnd 
  : public COpenGLCtrl
{
public:
  CVolumeWnd(CString Title=_T("OpenGL Window"), int xSize=640, int ySize=480, CWnd* pParent=NULL);
  virtual ~CVolumeWnd(void);

protected:
  void OnPrepareRC();
  void CustomizePixelFormat(PIXELFORMATDESCRIPTOR* pfd);
  void OnRender();
  virtual void ResizeOpenGLWindow(int cx, int cy);

protected:
  CWnd *pMyParent;
  CString myTitle;
  int xPos, yPos, width, height;
  CPoint origin;
	CGrCamera m_camera;

  int volume_size_x;
  int volume_size_y;
  int volume_size_z;
  double aspect_x;
  double aspect_y;
  float mat[16];
  double iso;
  double red, green, blue;

  vector<GLubyte> dataset;
  GLuint volume_tex;
  GLuint shaded_v;
  GLuint shaded_f;
  GLuint shaded_p;
  GLuint flat_p;
  GLint  light1Param, light2Param;
  GLfloat dx, dy, dz;
  struct HPMCConstants* hpmc_c;
  struct HPMCHistoPyramid* hpmc_h;
  struct HPMCTraversalHandle* hpmc_th_shaded;
  bool bShowFrame;

public:
  bool LoadVolume(CString sPathName);
  bool Init();
  void SetShowFrame(int ShowFrame);
  void SetIso(float newIso);
  void SetRed(float newRed);
  void SetGreen(float newGreen);
  void SetBlue(float newBlue);
  float GetIso();
  float GetRed();
  float GetGreen();
  float GetBlue();
  DECLARE_MESSAGE_MAP()
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
