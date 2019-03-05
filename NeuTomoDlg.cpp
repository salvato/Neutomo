//////////////////////////////////////////
// NeuTomoDlg.cpp : implementation file //
//////////////////////////////////////////

#include "stdafx.h"
#include <process.h> // _beginthread, _endthread
#include "float.h"
#include "math.h"
#include "NeuTomo.h"
#include "NeuTomoUtil.h"
#include "PlayCntlDlg.h"
#include "RoiDlg.h"
#include "Slices ProjectionDlg.h"
#include "3DToolsDlg.h"
#include "VolumeWnd.h"
#include "GrabberWnd.h"
#include "AlignWnd.h"
#include "AboutDlg.h"
#include "SliceThread.h"
#include "ParallelTomoThread.h"
#include "TomoProjection.h"
#include "CenterDlg.h"
#include "NeuTomoDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_GPU 10


void __cdecl RunTomoThread(void *pParent);


void __cdecl
RunTomoThread(void* pParent) {
	CNeuTomoDlg *pDlg = (CNeuTomoDlg *)pParent;
  if(pDlg == NULL) {
	  pDlg->hThread = 0;  // We have Done !
    pDlg->PostMessage(MSG_TOMO_DONE);
    _endthread();
  }
	pDlg->SetEscape(0);       // Prepare for Elaboration
  if(pDlg->GetBeamGeometry() == PARALLEL_BEAM)
    DoParallelTomo(pParent);
  else
    DoParallelTomo(pParent);
	pDlg->hThread = 0;  // We have Done !
  pDlg->PostMessage(MSG_TOMO_DONE);
  _endthread();
}


////////////////////////
// CNeuTomoDlg dialog //
////////////////////////


CNeuTomoDlg::CNeuTomoDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CNeuTomoDlg::IDD, pParent)
  , pGrabberWnd(NULL)
  , pAlignWnd(NULL)
  , pCenterDlg(NULL)
  , pMsg(NULL)
  , pRoiDlg(NULL)
  , pVolumeWnd(NULL)
  , pDlg3DTools(NULL)
  , pPlayCntlDlg(NULL)
  , b3DEnabled(true)
  , bDoTomoEnabled(true)
  , hThread(0)
  , xControl(0)
  , yControl(0)
  , sVersion(__TIMESTAMP__)

{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  ReadDlgPos();
  pAngles    = NULL;
  pPathNames = NULL;
}


CNeuTomoDlg::~CNeuTomoDlg() {
  if(hThread) {// Wait thread end
    CWaitCursor WaitCursor;
		WaitForSingleObject(hThread, 3000);
		CloseHandle(hThread);
	}

  if(pDlg3DTools)  delete pDlg3DTools;
  if(pPlayCntlDlg) delete pPlayCntlDlg;
  if(pGrabberWnd)  delete pGrabberWnd;
  if(pRoiDlg)      delete pRoiDlg; 
  if(pVolumeWnd)   delete pVolumeWnd;
  if(pAlignWnd)    delete pAlignWnd;
  if(pCenterDlg)   delete pCenterDlg;
  if(pPathNames)   delete[] pPathNames;
  if(pAngles)      delete[] pAngles;
  if(pMsg)         delete pMsg;
}


void 
CNeuTomoDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PREPROCESS,  buttonPreProcess);
  DDX_Control(pDX, IDC_STATUSMSG,   editStatus);
  DDX_Control(pDX, IDC_DOTOMO,      buttonDoTomo);
  DDX_Control(pDX, IDC_SHOW_SLICES, buttonShowSlices);
  DDX_Control(pDX, IDC_3DSHOW,      button3DShow);
  DDX_Control(pDX, IDCANCEL, buttonExit);
  DDX_Control(pDX, IDC_MAKE_PHANTOM, buttonMakePhantom);
}


BEGIN_MESSAGE_MAP(CNeuTomoDlg, CDialog)
  ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

  ON_BN_CLICKED(IDC_PREPROCESS,   &CNeuTomoDlg::OnBnClickedPreProcess)
  ON_BN_CLICKED(IDC_DOTOMO,       &CNeuTomoDlg::OnBnClickedDoTomo)
  ON_BN_CLICKED(IDC_SHOW_SLICES,  &CNeuTomoDlg::OnBnClickedShowSlices)
  ON_BN_CLICKED(IDC_MAKE_PHANTOM, &CNeuTomoDlg::OnBnClickedMakeNewPhantom)
  ON_BN_CLICKED(IDC_3DSHOW,       &CNeuTomoDlg::OnBnClicked3DShow)
  ON_BN_CLICKED(IDCANCEL,         &CNeuTomoDlg::OnBnClickedCancel)

  ON_MESSAGE(IMAGE_SELECTION_DONE,    &CNeuTomoDlg::OnSelectionDone)
  ON_MESSAGE(IMAGE_SELECTION_CHANGED, &CNeuTomoDlg::OnSelectionChanged)

  ON_MESSAGE(MSG_CHANGE_RED,          &CNeuTomoDlg::OnRedChanged)
  ON_MESSAGE(MSG_CHANGE_GREEN,        &CNeuTomoDlg::OnGreenChanged)
  ON_MESSAGE(MSG_CHANGE_BLUE,         &CNeuTomoDlg::OnBlueChanged)
  ON_MESSAGE(MSG_CHANGE_ISO,          &CNeuTomoDlg::OnIsoChanged)
  ON_MESSAGE(MSG_SHOW_FRAME,          &CNeuTomoDlg::OnShowFrame)
  ON_MESSAGE(MSG_CLOSE_VOLUME,        &CNeuTomoDlg::OnCloseVolWnd)

  ON_MESSAGE(MSG_STATUS_MESSAGE,      &CNeuTomoDlg::OnStatusMessage)

  ON_MESSAGE(MSG_PLAY,          &CNeuTomoDlg::OnPlaySlices)
  ON_MESSAGE(MSG_STOP,          &CNeuTomoDlg::OnStopSlices)
  ON_MESSAGE(MSG_PREV,          &CNeuTomoDlg::OnPreviousSlice)
  ON_MESSAGE(MSG_NEXT,          &CNeuTomoDlg::OnNextSlice)
  ON_MESSAGE(MSG_CLOSE_PLAY,    &CNeuTomoDlg::OnClosePlay)

  ON_MESSAGE(MSG_ALIGN_DONE,       &CNeuTomoDlg::OnAlignDone)
  ON_MESSAGE(MSG_ALIGN_CHANGED,    &CNeuTomoDlg::OnAlignChanged)
  ON_MESSAGE(MSG_ALIGN_ABORTED,    &CNeuTomoDlg::OnAlignAborted)

  ON_MESSAGE(MSG_TOMO_DONE,     &CNeuTomoDlg::OnTomoDone)

END_MESSAGE_MAP()


void
CNeuTomoDlg::SaveDlgPos() {
  if(!IsIconic()) {
    CRect Rect;
    GetWindowRect(Rect);
    AfxGetApp()->WriteProfileInt(__KEY__, _T("MainWnd X0"), Rect.TopLeft().x);
    AfxGetApp()->WriteProfileInt(__KEY__, _T("MainWnd Y0"), Rect.TopLeft().y);
  }
}


void
CNeuTomoDlg::ReadDlgPos() {
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  int xx0 = AfxGetApp()->GetProfileInt(__KEY__, _T("MainWnd X0"), xControl);
  int yy0 = AfxGetApp()->GetProfileInt(__KEY__, _T("MainWnd Y0"), yControl);
  if((xx0>0) && (yy0>0) && (xx0<rDeskTop.right-20) && (yy0<rDeskTop.bottom-20)) {
    xControl = xx0;
    yControl = yy0;
  } else {
    xControl = 0;
    yControl = 0;
  }
}


bool
CNeuTomoDlg::CheckGL() {
  _glutInit();
  GLuint glutWindowHandle = glutCreateWindow("Hidden Window");

  int err = glewInit();
  if(err != GLEW_OK) {// sanity check
    AfxMessageBox(CString((char *)glewGetErrorString(err)), MB_ICONSTOP);
    glutDestroyWindow(glutWindowHandle);
    return false;
  } // if(GLEW_OK != err)
  
  CString sString;

  bDoTomoEnabled = true;
  // Here is the place to check Graphic card capabilities !
  // After the GL context has been initialized
  char* cVersion1   = (char*)glGetString(GL_VERSION);
  char* cRenderer1  = (char*)glGetString(GL_RENDERER);
  char* cExtensions = (char*)glGetString(GL_EXTENSIONS);
  if(pMsg) {
    pMsg->AddText(_T("OpenGL Version: ") + CString(cVersion1) + _T("\r\n"));
    pMsg->AddText(_T("Graphic Adapter: ") + CString(cRenderer1) + _T("\r\n"));
  }

/*  
  if(!initCG()) bDoTomoEnabled = false;
  GLuint fbo;
  if(!initFBO(&fbo)) bDoTomoEnabled = false;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT,   1);
  // Create the 3 textures for the FBO
  GLuint outTexID[3];
  glGenTextures(3, outTexID);
  GLenum attachmentpoints[3];
  attachmentpoints[0] = GL_COLOR_ATTACHMENT0;
  attachmentpoints[1] = GL_COLOR_ATTACHMENT1;
  attachmentpoints[2] = GL_COLOR_ATTACHMENT2;
  glGenTextures(3, outTexID);
  if(!initFBOTextures(3, outTexID, 1, 2, attachmentpoints)) bDoTomoEnabled = false;


  if(!isExtensionSupported("GL_ARB_texture_rectangle") ||
     !isExtensionSupported("GL_ARB_texture_float")       // for GL_LUMINANCE32F_ARB support
    ) {
    bDoTomoEnabled = false;
  } else {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    GLuint TexID[3];
    glGenTextures(3, TexID);
    GLenum attachmentpoints[3];
    attachmentpoints[0] = GL_COLOR_ATTACHMENT0;
    attachmentpoints[1] = GL_COLOR_ATTACHMENT1;
    attachmentpoints[2] = GL_COLOR_ATTACHMENT2;
    for(int i=0; i<3; i++) {
      glBindTexture(GL_TEXTURE_RECTANGLE_ARB, TexID[i]);
      CheckGLErrors(_T("1"));
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S,     GL_CLAMP);
      CheckGLErrors(_T("2"));
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T,     GL_CLAMP);
      CheckGLErrors(_T("3"));
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      CheckGLErrors(_T("4"));
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      CheckGLErrors(_T("5"));
      glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_LUMINANCE32F_ARB, 128, 128, 0, GL_LUMINANCE, GL_FLOAT, 0);
      if(!CheckGLErrors(_T("CheckGL()"))) {
      //if(glGetError() != GL_NO_ERROR) {
        bDoTomoEnabled =  false;
        break;
      } else {// Attach the texture to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentpoints[i], GL_TEXTURE_RECTANGLE_ARB, TexID[i], 0);
        if(!CheckFramebufferStatus()) {
          bDoTomoEnabled =  false;
          break;
        }
      }
    }
    glDeleteTextures(3, TexID);
      if(bDoTomoEnabled) {
      // Test for multiple GPU support
      if(wglGetExtensionsStringARB == NULL) {
        ::DestroyWindow(hwnd);
        return false;
      }
      const char *extensionsString = wglGetExtensionsStringARB(hdc);
     if(strstr(extensionsString, "WGL_NV_gpu_affinity")) {
        int gpuIndex = 0;
        HGPUNV hGPU[MAX_GPU];
        // Get a list of the first MAX_GPU GPUs in the system
        while((gpuIndex < MAX_GPU) && wglEnumGpusNV(gpuIndex, &hGPU[gpuIndex])) {
          gpuIndex++;
        }
        sString.Format(_T("Multiple GPU Support Enabled: %d GPUs Presents"),gpuIndex);
        ShowStatusMsg(sString);
        if(pMsg) pMsg->AddText(sString + _T("\r\n"));
      } else {
        sString = _T("Multiple GPU Support Not Present");
        ShowStatusMsg(sString);
        if(pMsg) pMsg->AddText(sString + _T("\r\n"));
      }
      // End Test forMultiple GPU Support
    }
  }
*/

  glutDestroyWindow(glutWindowHandle);

  if(!bDoTomoEnabled) {
    sString = _T("Graphic card unable to perform Tomography !");
    ShowStatusMsg(sString);
    pMsg->AddText(sString + _T("\r\n"));
  }
  return true;
}


void
CNeuTomoDlg::DisableButtons() {
  buttonPreProcess.EnableWindow(false);
  buttonDoTomo.EnableWindow(false);
  button3DShow.EnableWindow(false);
  buttonShowSlices.EnableWindow(false);
  buttonMakePhantom.EnableWindow(false);
  buttonExit.EnableWindow(false);
}


void
CNeuTomoDlg::EnableButtons() {
  buttonPreProcess.EnableWindow(true);
  buttonDoTomo.EnableWindow(true);
  button3DShow.EnableWindow(true);
  buttonShowSlices.EnableWindow(true);
  buttonMakePhantom.EnableWindow(true);
  buttonExit.EnableWindow(true);
}


void
CNeuTomoDlg::EnableStopButton(){
  buttonExit.EnableWindow(true);
  buttonExit.SetWindowText(_T("Stop"));
}


void
CNeuTomoDlg::DisableStopButton(){
  buttonExit.EnableWindow(true);
  buttonExit.SetWindowTextW(_T("Exit"));
}


bool
CNeuTomoDlg::FindCenter(CProjection* pProjection0, CProjection* pProjection180) {
  bool bFindOk = true;
  pProjection180->HorizontalFlip();
  int imgWidth = pProjection0->n_columns;
  int imgHeight = pProjection0->n_rows;
  float* rm = new float[imgHeight];
  float* m  = new float[imgHeight];
  float* _x = new float[imgHeight];
  float* r = new float[imgWidth];
  ZeroMemory(m, sizeof(*m)*imgHeight);
  float *p1, *p2;
  float fMin, fMax, rMin, rMax;
  float rc;
  for(int row=0; row<imgHeight; row++) {
    _x[row] = row-0.5*imgHeight;
    p1 = pProjection0->pData+row*imgWidth;
    p2 = pProjection180->pData+row*imgWidth;
    CrossCorr(p1, p2, r, imgWidth, imgWidth);
    rMax = -FLT_MAX;
    for(int i=0; i<imgWidth; i++) {
      if(*(r+i) > rMax) {
        rMax    = *(r+i);
        rm[row] = rMax;
        m[row]  = double(i)-0.5*imgWidth;
      } // if(*(r+i) > rMax)
    } // for(i=0; i<imgWidth; i++)
  } // for(row=0; row<height; row++)
//#define __SHOW_PLOT
  #ifdef __SHOW_PLOT
    CPlotWindow Plot(_T("Cross Correlation"));
    Plot.ClearPlot();
    Plot.NewDataSet(1, 3, RGB(255,0,0), CPlotWindow::iline, _T(""));
    for(int i=0; i<imgHeight; i++) {
      Plot.NewPoint(1, double(i), double(m[i]));
    }
    Plot.SetShowDataSet(1,  true);
  #endif
  LinRegress(&_x[imgHeight/4], &m[imgHeight/4], imgHeight/2, &rotationCenter,  &tiltAngle, &rc);
  #ifdef __SHOW_PLOT
    Plot.NewDataSet(2, 3, RGB(255,255,0), CPlotWindow::iline, _T(""));
    Plot.NewPoint(2, 0.0, double(rotationCenter));
    Plot.NewPoint(2, imgHeight, double(rotationCenter+tiltAngle*imgHeight));
    Plot.SetShowDataSet(2,  true);
  #endif
  int n = 0;
  float* xx = new float[imgHeight];
  float* mm = new float[imgHeight];
  for(int i=0; i<imgHeight; i++) {
    if(fabs(rotationCenter+(tiltAngle)*_x[i]-m[i]) < 2) {
      xx[n] = _x[i];
      mm[n] = m[i];
      n++;
    } 
  }
  if(n > imgHeight/8) {
    LinRegress(xx, mm, n, &rotationCenter,  &tiltAngle, &rc);
    #ifdef __SHOW_PLOT
      Plot.NewDataSet(3, 3, RGB(255,255,255), CPlotWindow::iline, _T(""));
      Plot.NewPoint(3, double(xx[0]+0.5*imgWidth), double(rotationCenter+tiltAngle*xx[0]));
      Plot.NewPoint(3, double(xx[n-1]+0.5*imgWidth), double(rotationCenter+tiltAngle*xx[n-1]));
      Plot.SetShowDataSet(3,  true);
    #endif
  }
  #ifdef __SHOW_PLOT
    Plot.UpdatePlot();
  #endif
  if(xx != NULL) delete[] xx;
  if(mm != NULL) delete[] mm;
  if(_x != NULL) delete[] _x; _x = NULL;
  if(r  != NULL) delete[] r;
  if(m  != NULL) delete[] m; m= NULL;
  if(rm != NULL) delete[] rm;

  tiltAngle   = atan(tiltAngle)*90.0/M_PI;
  rotationCenter = 0.5*(rotationCenter);
  CProjection p0, p180;
  p0.Copy(*pProjection0);
  p180.Copy(*pProjection180);
  p0.GetMinMax(&fMin, &fMax);
  p180.GetMinMax(&rMin, &rMax);
  if((fMax-fMin == 0.0) ||(rMax-rMin==0.0))
    return false;
  float f_in;
  for(int i=0; i<p0.n_columns*p0.n_rows; i++) {
    f_in = (*(p0.pData+i)-fMin)/(fMax-fMin);
    //f_in = 0.5*sin(M_PI*f_in-(M_PI*.5))+1.0;
    *(p0.pData+i) = f_in;
    f_in = (*(p180.pData+i)-fMin)/(fMax-fMin);
    ///f_in = 0.5*sin(M_PI*f_in-(M_PI*.5))+1.0;
    *(p180.pData+i) = f_in;
  }

  if(pAlignWnd) delete pAlignWnd;
  pAlignWnd = NULL;
  pAlignWnd = new CAlignWnd(this, _T("Aligning Window"));
  
  if(pCenterDlg) delete pCenterDlg;
  pCenterDlg = NULL;
  pCenterDlg = new CCenterDlg(this);

  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  int ratio = 1;
  if((rDeskTop.right < p0.n_columns) ||
     (rDeskTop.bottom < p0.n_rows)) {
    double ratio1 = double(p0.n_columns)/double(rDeskTop.right-100);
    double ratio2 = double(p0.n_rows)/double(rDeskTop.bottom-100);
    ratio1 = ceil(ratio1);
    ratio2 = ceil(ratio2);
    ratio = int(max(ratio1, ratio2) + 0.5);
  }
  pAlignWnd->Initialize(p0.n_columns/ratio, p0.n_rows/ratio);
  pAlignWnd->LoadTextures(p0.pData, p180.pData, p0.n_columns, p0.n_rows);
  pAlignWnd->SetTilt(tiltAngle);
  pAlignWnd->SetPos(rotationCenter);  
  pAlignWnd->SetEndMessage(MSG_ALIGN_DONE);
  pAlignWnd->SetAbortMessage(MSG_ALIGN_ABORTED);
  pAlignWnd->SetAlignmentChangedMessage(MSG_ALIGN_CHANGED);
  pAlignWnd->ShowWindow(SW_SHOW);
  pAlignWnd->UpdateWindow();

  CString sString;
  sString.Format(_T("%.3f"), -rotationCenter);
  pCenterDlg->editCenter.SetWindowText(sString);
  sString.Format(_T("%.3f"), -0.5*tiltAngle);
  pCenterDlg->editTilt.SetWindowText(sString);
  pCenterDlg->ShowWindow(SW_SHOW);
  pCenterDlg->UpdateWindow();

  return bFindOk;
}


// To build a new projection as a sum of all
// the projection images in a Directory
bool
CNeuTomoDlg::BuildImgSum(CString sDirectory, CProjection* pImgSum) {
  WIN32_FIND_DATA FileData;// Data structure that describes the file found
  HANDLE hSearch;
  CString sPathName, sFileDati;
  CProjection CurrProj;
  int nFiles = 0;
  sPathName = sDirectory + _T("\\*.fits");

  hSearch = FindFirstFile(sPathName, &FileData);
  if(hSearch == INVALID_HANDLE_VALUE) {
    AfxMessageBox(_T("No Projection Files Found in Directory\n" + sDirectory), MB_ICONSTOP);
    return false;
  }
  sFileDati = FileData.cFileName;
  if(!CurrProj.ReadFromFitsFile(sDirectory+_T("\\")+sFileDati)) {
    AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
    FindClose(hSearch);
    return false;
  }
  pImgSum->Copy(CurrProj);
  nFiles = 1;
  sStatus.Format(_T("Summed File: %s"), sFileDati);
  ShowStatusMsg(sStatus);
  
  while(FindNextFile(hSearch, &FileData)) {
    sFileDati = FileData.cFileName;
    if(!CurrProj.ReadFromFitsFile(sDirectory+_T("\\")+sFileDati)) {
      AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
      FindClose(hSearch);
      return false;
    }
    pImgSum->Sum(CurrProj);
    nFiles++;
    sStatus.Format(_T("Processed File: %s"), sFileDati);
    ShowStatusMsg(sStatus);
  }
  FindClose(hSearch);

  pImgSum->LinTransform(1.0/nFiles, 0.0);

  sStatus.Format(_T("File Reading Done !"));
  ShowStatusMsg(sStatus);
  return true;
}


void
CNeuTomoDlg::ShowProjection(CGrabberWnd* pWnd, CProjection* pImg) {
  int xMax, yMax;
  xMax = min((int)pWnd->Image.GetWidth(), pImg->n_columns);
  yMax = min((int)pWnd->Image.GetHeight(), pImg->n_rows);

  float fMax, fMin, fVal;
  pImg->GetMinMax(&fMin, &fMax);

  RGBTRIPLE *pRow;
  int yOff;

  for(int y=0; y<yMax; y++) {
    pRow = (RGBTRIPLE *)pWnd->Image.GetBits(y);
    yOff = pImg->n_columns * y;
    for(int x=0; x<xMax; x++) {
      fVal = (*(pImg->pData+yOff+x)-fMin)/(fMax-fMin);
      (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = int(255.5*fVal);
    }
  }
  pWnd->UpdateNow();
}


//////////////////////////////////
// CNeuTomoDlg message handlers //
//////////////////////////////////



int
CNeuTomoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;
  ReadDlgPos();

  return 0;
}


BOOL 
CNeuTomoDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
  pMsg = new CMsgWindow(_T("NeuTomo Messages"));

  if(!CheckGL()) {
    AfxMessageBox(_T("Unable to Initialize OpenGL !"), MB_ICONSTOP);
    return FALSE;
  }


  CAlignWnd* pAlignWnd = new CAlignWnd();
  delete pAlignWnd;
  pAlignWnd = NULL;

  buttonDoTomo.EnableWindow(bDoTomoEnabled);
  button3DShow.EnableWindow(b3DEnabled);
	
  CRect Rect;
  GetWindowRect(&Rect);
  Rect.SetRect(xControl, yControl, xControl+Rect.Width(), yControl+Rect.Height());
  MoveWindow(&Rect, true);

  CString sTitle;
  GetWindowText(sTitle);
  SetWindowText(sTitle + _T(" - Version of ") + sVersion);

  return TRUE;  // return TRUE  unless you set the focus to a control
}


void 
CNeuTomoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if((nID & 0xFFF0) == IDM_ABOUTBOX)	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialog::OnSysCommand(nID, lParam);
	}
}


void
CNeuTomoDlg::OnPaint() {
	if(IsIconic()) {
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR
CNeuTomoDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}


afx_msg LRESULT
CNeuTomoDlg::OnSelectionChanged(WPARAM wParam, LPARAM lParam) {
  CRect* pRect = (CRect *)lParam;
  CString sText;
  if(pGrabberWnd != NULL) {
    pGrabberWnd->SetSelection(pRect);
    double dVal, dSigma=0.0, dMean=0.0;
    int nPoints  = 0;
    for(int y=pRoiDlg->m_YMin; y<pRoiDlg->m_YMax; y++) {
      RGBTRIPLE *pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
      for(int x=pRoiDlg->m_XMin; x<pRoiDlg->m_XMax; x++) {
        dVal = pRow[x].rgbtRed;
        dMean += dVal;
        nPoints++;
      } // for(int x=pRoiDlg->m_XMin; x<pRoiDlg->m_XMax; x++)
    } // for(int y=pRoiDlg->m_YMin; y<pRoiDlg->m_YMax; y++)
    dMean /= double(nPoints);// Intensità del Fascio di Neutroni Incidente
    for(int y=pRoiDlg->m_YMin; y<pRoiDlg->m_YMax; y++) {
      RGBTRIPLE *pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
      for(int x=pRoiDlg->m_XMin; x<pRoiDlg->m_XMax; x++) {
        dVal = pRow[x].rgbtRed;
        dSigma += pow(dMean-dVal, 2);
      } // for(int x=pRoiDlg->m_XMin; x<pRoiDlg->m_XMax; x++)
    } // for(int y=pRoiDlg->m_YMin; y<pRoiDlg->m_YMax; y++)
    dSigma = sqrt(dSigma/double(nPoints));
    sText.Format(_T("%.2f"), dMean);
    pRoiDlg->editMean.SetWindowText(sText);
    sText.Format(_T("%.2f"), dSigma);
    pRoiDlg->editSigma.SetWindowText(sText); 
  } // if(pGrabberWnd != NULL)
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnSelectionDone(WPARAM wParam, LPARAM lParam) {
  CString sText;
  if(pRoiDlg != NULL) {
    if(IsWindow(pRoiDlg->m_hWnd)) {
      int tmp;
      CRect* pSelection = (CRect*)lParam;
      pRoiDlg->m_XMin = pSelection->left;
      pRoiDlg->m_XMax = pSelection->right;
      pRoiDlg->m_YMin = pSelection->bottom;
      pRoiDlg->m_YMax = pSelection->top;
      if(pRoiDlg->m_XMin > pRoiDlg->m_XMax) {
        tmp = pRoiDlg->m_XMin;
        pRoiDlg->m_XMin = pRoiDlg->m_XMax;
        pRoiDlg->m_XMax = tmp;
      }
      if(pRoiDlg->m_YMin > pRoiDlg->m_YMax) {
        tmp = pRoiDlg->m_YMin;
        pRoiDlg->m_YMin = pRoiDlg->m_YMax;
        pRoiDlg->m_YMax = tmp;
      }
      double dVal, dSigma=0.0, dMean=0.0;
      int nPoints  = 0;
      for(int y=int(pGrabberWnd->Image.GetHeight())-pRoiDlg->m_YMax; y<int(pGrabberWnd->Image.GetHeight())-pRoiDlg->m_YMin; y++) {
        RGBTRIPLE *pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
        for(int x=pRoiDlg->m_XMin; x<pRoiDlg->m_XMax; x++) {
          dVal = pRow[x].rgbtRed;
          dMean += dVal;
          nPoints++;
        } // for(int x=pROIDlg->m_XMin; x<pROIDlg->m_XMax; x++)
      } // for(int y=pROIDlg->m_YMin; y<pROIDlg->m_YMax; y++)
      dMean /= double(nPoints);// Intensità del Fascio di Neutroni Incidente
      for(int y=int(pGrabberWnd->Image.GetHeight())-pRoiDlg->m_YMax; y<int(pGrabberWnd->Image.GetHeight())-pRoiDlg->m_YMin; y++) {
        RGBTRIPLE *pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
        for(int x=pRoiDlg->m_XMin; x<pRoiDlg->m_XMax; x++) {
          dVal = pRow[x].rgbtRed;
          dSigma += pow(dMean-dVal, 2);
        } // for(int x=pROIDlg->m_XMin; x<pROIDlg->m_XMax; x++)
      } // for(int y=pROIDlg->m_YMin; y<pROIDlg->m_YMax; y++)
      dSigma = sqrt(dSigma/double(nPoints));
      sText.Format(_T("%.2f"), dMean);
      pRoiDlg->editMean.SetWindowText(sText);
      sText.Format(_T("%.2f"), dSigma);
      pRoiDlg->editSigma.SetWindowText(sText);
      pRoiDlg->UpdateData(false);
    }
  }
  return LRESULT(0);
}


void
CNeuTomoDlg::OnBnClickedMakeNewPhantom() {
  CSlicesProjectionDlg SlicesProjectionDlg;
  SlicesProjectionDlg.DoModal();
}


void
CNeuTomoDlg::OnBnClickedShowSlices() {
  if(SelectShowDirDlg.DoModal() != IDOK) return;
  if(!ReadProjections(SelectShowDirDlg.GetShowDir())) return;
  pPlayCntlDlg = new CPlayCntlDlg(this);
  if(pGrabberWnd != NULL) delete pGrabberWnd;
  int ratio=1;
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  if((rDeskTop.right < imgWidth) ||
     (rDeskTop.bottom < imgHeight)) {
    double ratio1 = double(imgWidth)/double(rDeskTop.right-100);
    double ratio2 = double(imgHeight)/double(rDeskTop.bottom-100);
    ratio1 = ceil(ratio1);
    ratio2 = ceil(ratio2);
    ratio = int(max(ratio1, ratio2) +0.5);
  }
  pGrabberWnd = new CGrabberWnd(_T("Slices"), imgWidth/ratio, imgHeight/ratio, this);
  pGrabberWnd->CreateImage(imgWidth, imgHeight, 24, CXIMAGE_FORMAT_BMP);
  grayMin = fMin;
  grayMax = fMax;
  iCurrentSliceShown = 0;
  if(!pPlayCntlDlg->Create(IDD_PLAYCNTL_DIALOG, pGrabberWnd)) {
    AfxMessageBox(_T("Unable to Create PlayCntlDlg"));
    return;
  }
  DisableButtons();
  pPlayCntlDlg->ShowWindow(SW_SHOW);
  ShowWindow(SW_MINIMIZE);
/*
  PlayCntlDlg.DoModal();

  OnStopSlices(WPARAM(0), LPARAM(0));
	EnableButtons();
  DisableStopButton();
  if(pPathNames != NULL) delete[] pPathNames;
  pPathNames = NULL;
  if(pAngles != NULL) delete[] pAngles;
  pAngles = NULL;
  if(pGrabberWnd != NULL) delete pGrabberWnd;
  pGrabberWnd = NULL;
  return;
*/
}


void 
CNeuTomoDlg::OnBnClickedPreProcess() {
  if(PreProcessDlg.DoModal() != IDOK) return;
  if(!CheckAndDeleteFiles(PreProcessDlg.GetNormDir())) return;

  CProjection ProjSum;
  int xSize=0, ySize=0, _xSize=0, _ySize=0;
  if(!BuildImgSum(PreProcessDlg.GetProjectionDir(), &ProjSum)) {
    CString sError = _T("Unable to read Projection files or File Error\nProbably no .fits Files in Directory\n");
    AfxMessageBox(sError+PreProcessDlg.GetProjectionDir(), MB_ICONSTOP);
    return;
  }
  float fMin, fMax;
  ProjSum.GetMinMax(&fMin, &fMax); 

  // Create a window (NOT greater than the display !)
  RGBTRIPLE* pRow;
  int yOff;
  int ratio = 1;
  if(pGrabberWnd != NULL) delete pGrabberWnd;
  RECT rDeskTop;
  ::GetWindowRect(::GetDesktopWindow(), &rDeskTop);
  if((rDeskTop.right < ProjSum.n_columns) ||
     (rDeskTop.bottom < ProjSum.n_rows)) {
    double ratio1 = double(ProjSum.n_columns)/double(rDeskTop.right-100);
    double ratio2 = double(ProjSum.n_rows)/double(rDeskTop.bottom-100);
    ratio1 = ceil(ratio1);
    ratio2 = ceil(ratio2);
    ratio = int(max(ratio1, ratio2) +0.5);
  }
  pGrabberWnd = new CGrabberWnd(_T("Choose RoI"), ProjSum.n_columns/ratio, ProjSum.n_rows/ratio, this);
  pGrabberWnd->CreateImage(ProjSum.n_columns, ProjSum.n_rows, 24, CXIMAGE_FORMAT_BMP);
  // Show the summed image
  for(int y=0; y<ProjSum.n_rows; y++) {
    pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
    yOff = ProjSum.n_columns * y;
    for(int x=0; x<ProjSum.n_columns; x++) {
      (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = int(255.5*(*(ProjSum.pData+yOff+x)-fMin)/(fMax-fMin));
    }
  }
  pGrabberWnd->UpdateNow();
  
  // Choose a RoI on the Projections
  if(pRoiDlg == NULL) pRoiDlg = new CRoiDlg(this);
  pRoiDlg->m_XMin = pRoiDlg->m_YMin = 0;
  pRoiDlg->m_XMax = ProjSum.n_columns-1;
  pRoiDlg->m_YMax = ProjSum.n_rows-1;
  pGrabberWnd->EnableSelection(true);
  pRoiDlg->sTitle ="Select The Region of Interest";
  
  if(pRoiDlg->DoModal() != IDOK) {
    delete pGrabberWnd;
    pGrabberWnd = NULL;
    return;
  }
  if(pRoiDlg->m_XMin < 0)                   pRoiDlg->m_XMin = 0;
  if(pRoiDlg->m_XMax > ProjSum.n_columns-1) pRoiDlg->m_XMax = ProjSum.n_columns-1;
  if(pRoiDlg->m_YMin < 0)                   pRoiDlg->m_YMin = 0;
  if(pRoiDlg->m_YMax > ProjSum.n_rows-1)    pRoiDlg->m_YMax = ProjSum.n_rows-1;
  int xStart = pRoiDlg->m_XMin;
  int xEnd   = pRoiDlg->m_XMax;
  int yStart = ProjSum.n_rows-pRoiDlg->m_YMax-1;// The selection returns Coordinates with the
  int yEnd   = ProjSum.n_rows-pRoiDlg->m_YMin-1;// Origin placed on the Top-Left corner. 
  
  ProjSum.Crop(xStart, xEnd, yStart, yEnd);
  xSize = ProjSum.n_columns;
  ySize = ProjSum.n_rows;
  ProjSum.GetMinMax(&fMin, &fMax);

  // Resize the window (NOT greater than the video !)
  if(pGrabberWnd != NULL) delete pGrabberWnd;
  if((rDeskTop.right < ProjSum.n_columns) ||
     (rDeskTop.bottom < ProjSum.n_rows)) {
    double ratio1 = double(ProjSum.n_columns)/double(rDeskTop.right-100);
    double ratio2 = double(ProjSum.n_rows)/double(rDeskTop.bottom-100);
    ratio1 = ceil(ratio1);
    ratio2 = ceil(ratio2);
    ratio = int(max(ratio1, ratio2) +0.5);
  }
  pGrabberWnd = new CGrabberWnd(_T("Choose a Pure Beam Region"), ProjSum.n_columns/ratio, ProjSum.n_rows/ratio, this);
  pGrabberWnd->CreateImage(ProjSum.n_columns, ProjSum.n_rows, 24, CXIMAGE_FORMAT_BMP);
  for(int y=0; y<ySize; y++) {
    pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
    yOff = xSize * y;
    for(int x=0; x<xSize; x++) {
      (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = int(255.5*(*(ProjSum.pData+yOff+x)-fMin)/(fMax-fMin));
    }
  }
  pGrabberWnd->UpdateNow();
  
  // Choose a ROI for the Pure Beam Intensity
  if(pRoiDlg == NULL) pRoiDlg = new CRoiDlg(this);
  pRoiDlg->m_XMin = pRoiDlg->m_YMin = 0;
  pRoiDlg->m_XMax = xSize-1;
  pRoiDlg->m_YMax = ySize-1;
  pGrabberWnd->EnableSelection(true);
  pRoiDlg->sTitle ="Select Pure Beam Region";
  bOpenBeamCorr = true;
  if(pRoiDlg->DoModal() != IDOK) {
    bOpenBeamCorr = false;
  }
  if(pRoiDlg->m_XMin < 0)       pRoiDlg->m_XMin = 0;
  if(pRoiDlg->m_XMax > xSize-1) pRoiDlg->m_XMax = xSize-1;
  if(pRoiDlg->m_YMin < 0)       pRoiDlg->m_YMin = 0;
  if(pRoiDlg->m_YMax > ySize-1) pRoiDlg->m_YMax = ySize-1;
  int x0 = pRoiDlg->m_XMin;
  int x1 = pRoiDlg->m_XMax;
  int y1 = ySize-pRoiDlg->m_YMin;// Be carefull ... y axis Origin
  int y0 = ySize-pRoiDlg->m_YMax;// is REVERSED !!!!!!!!!!!!!!!!!
  pGrabberWnd->EnableSelection(false);

  CProjection Dark;
  if(PreProcessDlg.GetUseDark()) {
    if(!BuildImgSum(PreProcessDlg.GetDarkDir(), &Dark)) {
      CString sError = _T("Unable to read Dark Files or File Error\nProbably No .fits Files in Directory");
      AfxMessageBox(sError, MB_ICONSTOP);
      return;
    }
    if(!Dark.Crop(xStart, xEnd, yStart, yEnd)) {
      AfxMessageBox(Dark.sErrorString, MB_ICONSTOP);
      return;
    }
    Dark.f_MedianThreshold = 0.0;
    if(PreProcessDlg.GetFilterDark()) Dark.Median(3);
  }

  CProjection Beam;
  if(PreProcessDlg.GetUseBeam()) {
    if(!BuildImgSum(PreProcessDlg.GetBeamDir(), &Beam)) {
      CString sError = _T("Unable to read Beam Files or File Error\nProbably No .fits Files in Directory");
      AfxMessageBox(sError, MB_ICONSTOP);
      return;
    }
    if(!Beam.Crop(xStart, xEnd, yStart, yEnd)) {
      AfxMessageBox(Beam.sErrorString, MB_ICONSTOP);
      return;
    }
    Beam.f_MedianThreshold = 0.0;
    if(PreProcessDlg.GetFilterBeam()) Beam.Median(3);
  }

  //>>>>>>>>>>>>>>>>>>>>>>CNormalizedProjection CurrProj;
  CProjection CurrProj;

  bool bFirst = true;
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  CString sPathName, sFileDati, sInputDirectory, sOutputDirectory, sFileOut, sAngle;

  sInputDirectory = PreProcessDlg.GetProjectionDir();
  sPathName = sInputDirectory + _T("\\*.fits");
  hSearch = FindFirstFile(sPathName, &FileData);
  if(hSearch == INVALID_HANDLE_VALUE) {
    AfxMessageBox(_T("Invalid Path\n")+sPathName, MB_ICONSTOP);
    return;
  }
  float angle=0.0, angle1= 0.0;
  float min, max, norm;
  bool bSuccess;

  // Create a window NOT greater than the video !
  int wWidth = xEnd-xStart, wHeight = yEnd-yStart;
  if((rDeskTop.right < wWidth) ||
     (rDeskTop.bottom < wHeight)) {
    double ratio1 = double(wWidth)/double(rDeskTop.right-100);
    double ratio2 = double(wHeight)/double(rDeskTop.bottom-100);
    ratio1 = ceil(ratio1);
    ratio2 = ceil(ratio2);
    ratio = int(max(ratio1, ratio2) +0.5);
  }
  if(pGrabberWnd != NULL) delete pGrabberWnd;
  pGrabberWnd = new CGrabberWnd(_T("Normalized Projections"), wWidth/ratio, wHeight/ratio, this);

  bool bFirstImageShown = true;
  bool bFirstProjectioRead = true;
  CProjection *pP1 = NULL;
  int shift = 0;
  if(PreProcessDlg.GetCorrectionAndorBug()) {
    pP1 = new CProjection();
  }

  do{
    sFileDati = FileData.cFileName;
    if(!CurrProj.ReadFromFitsFile(sInputDirectory+_T("\\")+sFileDati)) {
      AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
      if(pP1) delete pP1;
      return;
    }
    if(CurrProj.f_angle == CurrProj.FLOAT_UNDEFINED) {
      CurrProj.f_angle = angle;
      angle += PreProcessDlg.fAngularStep;
    }

    if(PreProcessDlg.GetCorrectionAndorBug()) {
      if(bFirstProjectioRead) {
        bFirstProjectioRead = false;
      } else {
        shift += EvaluateShift(pP1, &CurrProj);
      }
      pP1->Copy(CurrProj);
      if(shift) 
        CurrProj.ShiftRows(shift);
    }// if(PreProcessDlg.GetCorrectionAndorBug())

    if(!CurrProj.Crop(xStart, xEnd, yStart, yEnd)) {
      AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
      if(pP1) delete pP1;
      return;
    }
    
    // Pre Median Filtering ?
    if(PreProcessDlg.GetPreMedianFilter()) {
      if(!CurrProj.Median(3)) {
        AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
        if(pP1) delete pP1;
        return;
      }
    }

    // Do we Use Dark Subtraction ?
    bSuccess = true;
    if(PreProcessDlg.GetUseDark() && PreProcessDlg.GetUseDark()) {
      bSuccess = CurrProj.FlatFieldCorr(Beam, Dark);
    } else if(PreProcessDlg.GetUseBeam()) {
      bSuccess = CurrProj.FlatFieldCorr(Beam);
    }
    if(!bSuccess) {
      AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
      if(pP1) delete pP1;
      return;
    }

    // Do we have to normalize to the Open Beam Value ?
    if(bOpenBeamCorr) {
      if(!CurrProj.OpenBeamCorr(x0, x1, y0, y1)) {
        AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
        if(pP1) delete pP1;
        return;
      }
    } else {
      if(!CurrProj.OpenBeamCorr(1.0)) {
        AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
        if(pP1) delete pP1;
        return;
      }
    }

    // Post Median Filtering ?
    if(PreProcessDlg.GetPostMedianFilter()) {
      CurrProj.f_MedianThreshold = PreProcessDlg.fPostThreshold;
      if(!CurrProj.Median(3)) {
        AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
        if(pP1) delete pP1;
        return;
      }
    }

    // Show the Normalized Projection
    if(bFirstImageShown) {  
      pGrabberWnd->CreateImage(CurrProj.n_columns, CurrProj.n_rows, 24, CXIMAGE_FORMAT_BMP);
      bFirstImageShown = false;
      CurrProj.GetMinMax(&min, &max);
      norm = 255.0f/(max-min);
    }
    int iVal;
    for(int y=0; y<CurrProj.n_rows; y++) {
      pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
      yOff = CurrProj.n_columns * y;
      for(int x=0; x<CurrProj.n_columns; x++) {
        iVal = int(norm*(*(CurrProj.pData+yOff+x)-min));
        if(iVal < 0) 
          iVal = 0;
        if(iVal > 255) 
          iVal = 255;
        (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = iVal;
      }
    }
    pGrabberWnd->SetWindowText(sFileDati);
    pGrabberWnd->UpdateNow();
    sFileOut = sFileDati.Left(sFileDati.ReverseFind(_T('.'))) + _T(".fits");
    sOutputDirectory = PreProcessDlg.GetNormDir();
    sStatus.Format(_T("Writing File: %s at %.3f°"), sFileOut, CurrProj.f_angle);
    ShowStatusMsg(sStatus);
    if(!CurrProj.WriteToFitsFile(sOutputDirectory, sFileOut)) {
      AfxMessageBox(CurrProj.sErrorString, MB_ICONSTOP);
      return;
      if(pP1) delete pP1;
    }
    angle1 = angle;
    CurrProj.Reset();
  } while(FindNextFile(hSearch, &FileData));
  FindClose(hSearch);
  if(pP1) delete pP1;
}


bool
CNeuTomoDlg::ReadProjections(CString sInputDirectory) {
  CString sTmp;
  float tmp;
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  CString sPathName, sFileDati, sString;
  sPathName = sInputDirectory + _T("\\*.fits");
  CWaitCursor WaitCursor;
  nProjections = 0;
  
  // How many Projections do we have ?
  hSearch = FindFirstFile(sPathName, &FileData);
  if(hSearch == INVALID_HANDLE_VALUE) {
    AfxMessageBox(_T("Invalid Path\n")+sPathName, MB_ICONSTOP);
    return false;
  }
  do{
    nProjections++;
  } while(FindNextFile(hSearch, &FileData));
  FindClose(hSearch);
  if(nProjections == 0) {
    AfxMessageBox(_T("No Slice Files Found"), MB_ICONSTOP);
  }

  // Reserve an array of Pathname Pointers & angles
  if(pPathNames != NULL) delete[] pPathNames;
  pPathNames = new CString[nProjections];
  if(pAngles != NULL) delete[] pAngles;
  pAngles = new float[nProjections];
  CProjection Projection;
  fMin = FLT_MAX; fMax =-FLT_MAX;
  float rMin = FLT_MAX, rMax =-FLT_MAX;

  // Now read the projections in Memory
  hSearch = FindFirstFile(sPathName, &FileData);
  int nFiles = 0;
  try {
    do{
      sFileDati = FileData.cFileName;
      pPathNames[nFiles] = sInputDirectory+_T("\\")+sFileDati;
      if(!Projection.ReadFromFitsFile(pPathNames[nFiles])) {
        AfxMessageBox(Projection.sErrorString, MB_ICONSTOP);
        goto ErrorExit;
      }
      if(nFiles == 0) {
        imgWidth = Projection.n_columns;
        imgHeight = Projection.n_rows;
      }
      if((imgWidth != Projection.n_columns) || (imgHeight != Projection.n_rows)) {
        AfxMessageBox(_T("Different size Projections in Directory"), MB_ICONSTOP);
        goto ErrorExit;
      }
      pAngles[nFiles] = Projection.f_angle;
      Projection.GetMinMax(&rMin, &rMax);
      if(rMin < fMin) fMin = rMin;
      if(rMax > fMax) fMax = rMax;
      // Tell which file has been read
      sStatus.Format(_T("Read File: %s "), sFileDati);
      ShowStatusMsg(sStatus);
      // One more file
      Projection.Reset();
      nFiles++;
    } while(FindNextFile(hSearch, &FileData));
  } catch(...) {
    AfxMessageBox(_T("Error Reading Slice Files"), MB_ICONSTOP);
    goto ErrorExit;
  }
  FindClose(hSearch);

  sStatus.Format(_T("Slice Dimension: %dx%d - Imax=%g, Imin=%g"), imgWidth, imgHeight, fMax, fMin);
  ShowStatusMsg(sStatus);
  
  // Sort Files in Ascending Angle order
  for(int i=0; i<nProjections-1; i++) {
    for(int j=0; j<nProjections-1-i; j++)
      if(pAngles[j+1] < pAngles[j]) {
        tmp = pAngles[j];
        pAngles[j] = pAngles[j+1];
        pAngles[j+1] = tmp;
        sTmp = pPathNames[j];
        pPathNames[j] = pPathNames[j+1];
        pPathNames[j+1] = sTmp;
    }
  }
  return true;

ErrorExit:
  if(pPathNames != NULL) delete[] pPathNames;
  pPathNames = NULL;
  if(pAngles != NULL) delete[] pAngles;
  pAngles = NULL;
  FindClose(hSearch);
  return false;
}


void
CNeuTomoDlg::OnBnClickedDoTomo() {
  if(DoTomoDlg.DoModal() != IDOK) return;
  if(!CheckAndDeleteFiles(DoTomoDlg.GetSliceDir())) return;
  if(!ReadProjections(DoTomoDlg.GetNormDir())) return;
  DisableButtons();

  // Try to extimate Rotation Center and Tilt
  rotationCenter = 0.0;
  tiltAngle      = 0.0;
  int i180       = 0;
  float fDist    = FLT_MAX;
  for(int i=GetNProjections()-1; i>-1; i--) {
    if(fabs(GetAngles()[i]-180.0f) < fDist) {
      fDist = fabs(GetAngles()[i]-180.0f);
      i180  = i;
    }
  }
  if(fDist > 1.0) {
    CString sString;
    sString.Format(_T("Warning: distance between specular images = %.1f\r\n"), GetAngles()[i180]-GetAngles()[0]);
    pMsg->AddText(sString);
  }

  CProjection* pP0 = new CProjection();
  CProjection* pP180 = new CProjection();
  pP0->ReadFromFitsFile(GetPathNames()[0]);
  pP180->ReadFromFitsFile(GetPathNames()[i180]);
  if(!FindCenter(pP0, pP180)) {
    delete pP0;
    delete pP180;
    EnableButtons();
    return;
  }
  delete pP0;
  delete pP180;

  return;
}


afx_msg LRESULT
CNeuTomoDlg::OnAlignDone(WPARAM wParam, LPARAM lParam) {
  rotationCenter = pAlignWnd->GetPos();
  tiltAngle      = pAlignWnd->GetTilt();
  if(pAlignWnd)  delete pAlignWnd; pAlignWnd = NULL;
  if(pCenterDlg) delete pCenterDlg; pCenterDlg = NULL;
  //Start the Show Thread
  hThread=(HANDLE)_beginthread(RunTomoThread, 0, this);
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnAlignChanged(WPARAM wParam, LPARAM lParam) {
  rotationCenter = pAlignWnd->GetPos();
  tiltAngle      = pAlignWnd->GetTilt();
  CString sString;
  sString.Format(_T("%.3f"), -rotationCenter);
  pCenterDlg->editCenter.SetWindowText(sString);
  sString.Format(_T("%.3f"), -0.5*tiltAngle);
  pCenterDlg->editTilt.SetWindowText(sString);
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnAlignAborted(WPARAM wParam, LPARAM lParam) {
  if(pAlignWnd)  delete pAlignWnd; pAlignWnd = NULL;
  if(pCenterDlg) delete pCenterDlg; pCenterDlg = NULL;
	EnableButtons();
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnTomoDone(WPARAM wParam, LPARAM lParam) {
  DisableStopButton();
	EnableButtons();
  nProjections = 0;
  delete[] pAngles;    pAngles = NULL;
  delete[] pPathNames; pPathNames = NULL;
  return LRESULT(0);
}


void
CNeuTomoDlg::OnBnClicked3DShow() {
  if(SelectShowDirDlg.DoModal() != IDOK) return;
  pVolumeWnd = new CVolumeWnd(_T("3D Rendering"), 512, 512, this);
  if(pDlg3DTools == NULL) {
    pDlg3DTools = new C3DToolsDlg(this);
    if(!pDlg3DTools->Create(IDD_3DTOOLS_DIALOG, pVolumeWnd)) {
      AfxMessageBox(_T("Unable to Create 3D Tools Window"));
      delete pDlg3DTools; pDlg3DTools = NULL;
      return;
    }
  }

  if(!pVolumeWnd->LoadVolume(SelectShowDirDlg.GetShowDir())) {
    AfxMessageBox(_T("Unable to Load Volume"), MB_ICONEXCLAMATION);
    delete pVolumeWnd; pVolumeWnd = NULL;
    delete pDlg3DTools; pDlg3DTools = NULL;
    return;
  }
  pDlg3DTools->SetIntensity(int(pVolumeWnd->GetIso()*1000.0));
  if(!pVolumeWnd->Init()) {
    delete pVolumeWnd; pVolumeWnd = NULL;
    delete pDlg3DTools; pDlg3DTools = NULL;
    return;
  }
  DisableButtons();
  pVolumeWnd->ShowWindow(SW_SHOW);
  pDlg3DTools->ShowWindow(SW_SHOW);
  ShowWindow(SW_MINIMIZE);

/*
  if(pVolumeWnd == NULL) {
    pVolumeWnd = new CShowVolWnd(_T("3D Rendering"), 512, 512, this);
  }
  CString sInputDirectory = SelectShowDirDlg.GetShowDir();
  if(!pVolumeWnd->SendMessage(MSG_LOAD_VOLUME, 0, (LPARAM)&sInputDirectory)) {
    AfxMessageBox(_T("Unable to Load Volume"), MB_ICONEXCLAMATION);
    if(pVolumeWnd != NULL) delete pVolumeWnd;
    pVolumeWnd = NULL;
    EnableButtons();
    return;
  }
  pVolumeWnd->SetDensity(pDlg3DTools->GetDensity()/32.0f);
  pVolumeWnd->SetIntensity(pDlg3DTools->GetIntensity()/32.0f);
  pVolumeWnd->SetSliceN(pDlg3DTools->GetSliceN());
  pVolumeWnd->SetShownTexture(pDlg3DTools->GetShownTexture());
  pVolumeWnd->SetShowFrame(pDlg3DTools->GetShowFrame());
*/

}


afx_msg LRESULT 
CNeuTomoDlg::OnIsoChanged(WPARAM wParam, LPARAM lParam) {
  if(pVolumeWnd != NULL) {
    float iso = float(wParam) / 1000.0f;
    pVolumeWnd->SetIso(iso);
  }
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnRedChanged(WPARAM wParam, LPARAM lParam) {
  if(pVolumeWnd != NULL) {
    float red = float(wParam) / 100.0;
    pVolumeWnd->SetRed(red);
  }
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnGreenChanged(WPARAM wParam, LPARAM lParam) {
  if(pVolumeWnd != NULL) {
    float green = float(wParam) / 100.0;
    pVolumeWnd->SetGreen(green);
  }
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnBlueChanged(WPARAM wParam, LPARAM lParam) {
  if(pVolumeWnd != NULL) {
    float blue = float(wParam) / 100.0;
    pVolumeWnd->SetBlue(blue);
  }
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnStatusMessage(WPARAM wParam, LPARAM lParam) {
  sStatus.Format(_T("%s"), *(CString*)lParam);
  ShowStatusMsg(sStatus);
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnShowFrame(WPARAM wParam, LPARAM lParam) {
  if(pVolumeWnd != NULL) {
    pVolumeWnd->SetShowFrame((int)wParam);
  }
  return LRESULT(0);
}


void
CNeuTomoDlg::OnBnClickedCancel() {
  CString sString;
  buttonExit.GetWindowTextW(sString);
  if(sString == "Stop") {
    if(AfxMessageBox(_T("Stop Thread"), MB_YESNO) == IDYES)
      SetEscape(1);// Ask to stop the running thread
  } else {
    if(AfxMessageBox(_T("Terminate Program"), MB_YESNO) == IDYES) {
      OnCancel();
      SaveDlgPos();
    }
  }
}


afx_msg LRESULT
CNeuTomoDlg::OnPlaySlices(WPARAM wParam, LPARAM lParam) {
  //Start the Show Thread
  hThread=(HANDLE)_beginthread(RunShowSlicesThread, 0, this);
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnStopSlices(WPARAM wParam, LPARAM lParam) {
  //Stop the Show Thread
  SetEscape(1);
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnPreviousSlice(WPARAM wParam, LPARAM lParam) {
  iCurrentSliceShown--;
  if(iCurrentSliceShown < 0) iCurrentSliceShown = nProjections-1;
  CProjection Projection;
  Projection.ReadFromFitsFile(pPathNames[iCurrentSliceShown]);
  float* pData = Projection.pData;
  RGBTRIPLE *pRow; 
  int yOff;
  CString sString;
  sString.Format(_T("Slice# %d"), iCurrentSliceShown);
  pGrabberWnd->SetWindowText(sString);
  for(int y=0; y<imgHeight; y++) {
    pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
    yOff = imgWidth * y;
    for(int x=0; x<imgWidth; x++) {
      (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = int(255.5*(*(pData+yOff+x)-grayMin)/(grayMax-grayMin));
    }
  }
  pGrabberWnd->UpdateNow();
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnNextSlice(WPARAM wParam, LPARAM lParam) {
  iCurrentSliceShown = (iCurrentSliceShown+1) % nProjections;
  CProjection Projection;
  Projection.ReadFromFitsFile(pPathNames[iCurrentSliceShown]);
  float* pData = Projection.pData;
  RGBTRIPLE *pRow; 
  int yOff;
  CString sString;
  sString.Format(_T("Slice# %000d"), iCurrentSliceShown);
  pGrabberWnd->SetWindowText(sString);
  for(int y=0; y<imgHeight; y++) {
    pRow = (RGBTRIPLE *)pGrabberWnd->Image.GetBits(y);
    yOff = imgWidth * y;
    for(int x=0; x<imgWidth; x++) {
      (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = int(255.5*(*(pData+yOff+x)-grayMin)/(grayMax-grayMin));
    }
  }
  pGrabberWnd->UpdateNow();
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnClosePlay(WPARAM wParam, LPARAM lParam) {
  OnStopSlices(WPARAM(0), LPARAM(0));
  DisableStopButton();
	EnableButtons();
  nProjections = 0;
  if(pPathNames != NULL) delete[] pPathNames;
  pPathNames = NULL;
  if(pAngles != NULL) delete[] pAngles;
  pAngles = NULL;
  if(pGrabberWnd != NULL) delete pGrabberWnd;
  pGrabberWnd = NULL;
  if(pPlayCntlDlg != NULL) delete[] pPlayCntlDlg;
  pPlayCntlDlg = NULL;
  ShowWindow(SW_RESTORE);
  SetFocus();
  return LRESULT(0);
}


afx_msg LRESULT
CNeuTomoDlg::OnCloseVolWnd(WPARAM wParam, LPARAM lParam) {
  if(pDlg3DTools != NULL) {
    delete pDlg3DTools;
    pDlg3DTools = NULL;
  }
  if(pVolumeWnd != NULL) {
    delete pVolumeWnd;
    pVolumeWnd = NULL;
  }
  ShowWindow(SW_RESTORE);
  SetFocus();
  DisableStopButton();
  EnableButtons();
  return LRESULT(0);
}


void
CNeuTomoDlg::ShowStatusMsg(CString sMsg) {
  RECT rClient;
  editStatus.SetWindowText(sMsg);
  editStatus.GetClientRect(&rClient);
  RedrawWindow(&rClient, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}


bool
CNeuTomoDlg::CheckAndDeleteFiles(CString sPath) {
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  CString sPathName, sFileDati, FileOut, sString;
  sPathName = sPath + _T("\\*.fits");
  // Do we have Projections in that directory ?
  hSearch = FindFirstFile(sPathName, &FileData);
  if(hSearch == INVALID_HANDLE_VALUE) return true;// No files: OK
  if(AfxMessageBox(_T("Directory Not Empty\n Ok to Overwite ?"), MB_OKCANCEL) != IDOK) return false;
  do{
    sFileDati = FileData.cFileName;
    if(!DeleteFile(sPath+_T("\\")+sFileDati)) {
      AfxMessageBox(_T("Unable to delete fiel:\n")+sFileDati, MB_ICONSTOP);
      FindClose(hSearch);
      return false;
    }
  } while(FindNextFile(hSearch, &FileData));
  FindClose(hSearch);

  return true;
}


FILTER_TYPES
CNeuTomoDlg::GetFilterType(void) {
  return DoTomoDlg.GetFilterType();
}


bool
CNeuTomoDlg::GetFilterSinograms(void) {
  return DoTomoDlg.GetFilterSinograms();
}


CString
CNeuTomoDlg::GetSliceDir(void) {
  return DoTomoDlg.GetSliceDir();
}


BEAM_GEOMETRY 
CNeuTomoDlg::GetBeamGeometry(void) {
  return  DoTomoDlg.GetBeamGeometry();
}


CString* 
CNeuTomoDlg::GetPathNames(void) {
  return pPathNames;
}


float*
CNeuTomoDlg::GetAngles(void) {
  return pAngles;
}


int
CNeuTomoDlg::GetNProjections(void) {
  return nProjections;
}


int
CNeuTomoDlg::GetImgHeight(void) {
  return imgHeight;
}


int
CNeuTomoDlg::GetImgWidth(void) {
  return imgWidth;
}


int
CNeuTomoDlg::GetCurrentSlice(void) {
  return iCurrentSliceShown;
}


void
CNeuTomoDlg::SetCurrentSlice(int iSlice) {
  iCurrentSliceShown = iSlice;
}


float
CNeuTomoDlg::GetGrayMin(void) {
  return grayMin;
}


float
CNeuTomoDlg::GetGrayMax(void) {
  return grayMax;
}


void
CNeuTomoDlg::SetEscape(int DoEscape) {
	iEscape = DoEscape;
}


int
CNeuTomoDlg::GetEscape(void) {
  return iEscape;
}


float
CNeuTomoDlg::GetSourceObjectDistance(void) {
  return DoTomoDlg.GetSourceSampleDistance() ;
}


float
CNeuTomoDlg::GetSourceDetectorDistance(void) {
  return DoTomoDlg.GetSourceDetectorDistance();
}


float
CNeuTomoDlg::GetDetectorWidth(void) {
  return DoTomoDlg.GetHorizzontalFOV();
}


float
CNeuTomoDlg::GetDetectorHeight(void) {
  return DoTomoDlg.GetVerticalFOV();
}


int
CNeuTomoDlg::GetVolXSize() {
  return DoTomoDlg.GetVolXSize();
}


int
CNeuTomoDlg::GetVolYSize() {
  return DoTomoDlg.GetVolYSize();
}


int
CNeuTomoDlg::GetVolZSize() {
  return DoTomoDlg.GetVolZSize();
}


int
CNeuTomoDlg::EvaluateShift(CProjection* pP1, CProjection* pP2) { 
  int nrows     = pP1->n_rows;
  int ncolumns = pP1->n_columns;
  if(nrows     != pP2->n_rows) return 10.0;
  if(ncolumns != pP2->n_columns) return 10.0;
  double sum0, sum_1, sum1;
  sum0 = sum_1 = sum1 = 0.0;
  // Lets try the whole image first
  for(int i=0; i<nrows*ncolumns; i++) {
    sum0 += (pP1->pData[i] - pP2->pData[i]) * (pP1->pData[i] - pP2->pData[i]);
  }
  sum0 /= double(nrows*ncolumns);
  // Then with the image 2 translated up one row
  for(int i=0; i<(nrows-1)*ncolumns; i++) {
    sum1 += (pP1->pData[i] - pP2->pData[i+ncolumns])*(pP1->pData[i] - pP2->pData[i+ncolumns]);
  }
  sum1 /= double((nrows-1)*ncolumns);
  // Then with the image 1 translated up one row
  for(int i=0; i<(nrows-1)*ncolumns; i++) {
    sum_1 += (pP1->pData[i+ncolumns] - pP2->pData[i])*(pP1->pData[i+ncolumns] - pP2->pData[i]);
  }
  sum_1 /= double((nrows-1)*ncolumns);
  if(sum0 <= sum1) {
    if(sum0 <= sum_1) { 
      return 0;
    } else {
      return -1;
    }
  } else if(sum1 < sum_1) {
    return 1;
  } else { 
    return -1;
  }
}
