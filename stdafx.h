// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


// Registry Key
#define __KEY__ _T("TomoData")


//NVidia GPU Memory Support
#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B


// File Formats
enum
ENUM_PROJECTION_FORMATS {
  PROJECTION_FORMAT_BMP             = 0,
  PROJECTION_FORMAT_FITS            = 1,
  PROJECTION_FORMAT_BINARY          = 2,
  PROJECTION_FORMAT_MODIFIED_BINARY = 3,
  PROJECTION_FORMAT_JPEG            = 4
};


enum
FILTER_TYPES {
  SHEPP_LOGAN = 0,
  RAM_LAK     = 1
};


enum
BEAM_GEOMETRY {
  PARALLEL_BEAM = 0,
  CONE_BEAM     = 1
};


// Messages
#define IMAGE_SELECTION_DONE    WM_USER
#define IMAGE_SELECTION_CHANGED WM_USER + 1

#define MSG_LOAD_VOLUME         WM_USER + 10
#define MSG_STATUS_MESSAGE      WM_USER + 15
#define MSG_CLOSE_VOLUME        WM_USER + 17

#define MSG_PLAYCNTL            WM_USER + 20 //Uses at least 4 Messages

#define MSG_CHANGE_RED          WM_USER + 30
#define MSG_CHANGE_GREEN        WM_USER + 31
#define MSG_CHANGE_BLUE         WM_USER + 32
#define MSG_CHANGE_ISO          WM_USER + 33
#define MSG_SHOW_FRAME          WM_USER + 39

#define MSG_TOMO_DONE           WM_USER + 40

#define MSG_ALIGN_DONE          WM_USER + 50
#define MSG_ALIGN_CHANGED       WM_USER + 51
#define MSG_ALIGN_ABORTED       WM_USER + 52



