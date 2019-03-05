#pragma once

#include "stdafx.h"
#include "ximage.h"
#include ".\GrabberWnd.h"
#include "WindowTip.h"

////////////////////////////////////////////////////////
// CGrabberWnd
////////////////////////////////////////////////////////

#define IMAGE_SELECTION_DONE WM_USER

class
CGrabberWnd : public CWnd {
	DECLARE_DYNAMIC(CGrabberWnd)

public:
	CGrabberWnd(CString Title=_T("Frame Gabber"), int xSize=640, int ySize=480, CWnd* pParent=NULL);
	virtual ~CGrabberWnd();

private:
  int x0, y0, wdth, hght;
  double xRatio, yRatio;
  CString ClassName;
  CString strSection;
  CString strStringItem;
  CString strValue;
  CString sText;
  CWnd* myParent;
  bool bSelectionEnabled;
  CWindowTip myTip;
  INT_PTR TipPointer;
  CPoint LastPoint;

protected:
	DECLARE_MESSAGE_MAP()
  bool Zooming;
  CRect Selection, ScaledSelection;
	HCURSOR myCursor;
	HCURSOR oldCursor;


public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  CxImage Image;
  afx_msg void OnPaint();
  int Save(CString sFileName);
  int ClearImage(void);
  void* CreateImage(int xSize, int ySize, DWORD wBpp, DWORD imagetype=0);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT, CPoint);
  afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnCaptureChanged(CWnd *pWnd);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnTimer(UINT nIDEvent);
  void EnableSelection(bool bEnable);
  bool IsSelectionEnabled();

  void UpdateNow(void);
  void SetSelection(CRect* pRect);
};


