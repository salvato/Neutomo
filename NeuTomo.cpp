// NeuTomo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NeuTomo.h"
#include "MsgWindow.h"
#include "NeuTomoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////
//Libraries to load... //
/////////////////////////

#pragma comment (lib, "cg.lib")         // To Load the OpenGL
#pragma comment (lib, "cgGL.lib")       // To load the OpenGL Shading Language Extension
#pragma comment (lib, "glew32.lib")     // To Load the OpenGL Extensions
#pragma comment (lib, "cfitsio.lib")    // To Load the FITS Library to Read PSI Files
#pragma comment (lib, "glew32.lib")     // To Load the OpenGL Extensions
#ifdef _DEBUG                           // To Load the CxImage and Plot2 Libraries
  #pragma comment (lib, "HPMCd.lib")    // To Load the 3D visualization Library
  #ifdef _UNICODE
    #pragma comment (lib, "cximagedu.lib")
    #pragma comment (lib, "Plot3ud.lib")
  #else
    #pragma comment (lib, "cximaged.lib")
    #pragma comment (lib, "Plot3d.lib")
  #endif
#else
  #pragma comment (lib, "HPMC.lib")
  #ifdef _UNICODE
    #pragma comment (lib, "cximageu.lib")
    #pragma comment (lib, "Plot3u.lib")
  #else
    #pragma comment (lib, "cximage.lib")
    #pragma comment (lib, "Plot3.lib")
  #endif
#endif

// CNeuTomoApp

BEGIN_MESSAGE_MAP(CNeuTomoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNeuTomoApp construction

CNeuTomoApp::CNeuTomoApp() {
 }


// The one and only CNeuTomoApp object
CNeuTomoApp theApp;


// CNeuTomoApp initialization

BOOL 
CNeuTomoApp::InitInstance() {
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	SetRegistryKey(_T("Gabriele Salvato"));

	CNeuTomoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	} else if (nResponse == IDCANCEL) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
