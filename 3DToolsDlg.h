#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// finestra di dialogo C3DToolsDlg

class C3DToolsDlg : public CDialog
{
	DECLARE_DYNAMIC(C3DToolsDlg)

public:
	C3DToolsDlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~C3DToolsDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_3DTOOLS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV

private:
  CWnd* pMyParent;
  CWnd* pVolwND;
	int xControl, yControl;

	DECLARE_MESSAGE_MAP()
public:
  CSliderCtrl sliderIntensity;
  void SetRed(int Red);
  void SetGreen(int Green);
  void SetBlue(int Blue);
  void SetIntensity(int Intensity);
  int GetRed();
  int GetGreen();
  int GetBlue();
  int GetIntensity();
  void GetIntensityRange(int& min, int& max);
  int GetShownTexture();
  int GetShowFrame();

  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnBnClickedShowFrame();
  virtual BOOL OnInitDialog();
  CButton buttonShowFrame;
  afx_msg void OnClose();
  CSliderCtrl sliderRed;
  CSliderCtrl sliderGreen;
  CSliderCtrl sliderBlue;
};
