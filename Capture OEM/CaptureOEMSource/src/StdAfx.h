#pragma once


// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


// System requirements to run this application:
// Bugzilla.1026
//    For VS215 or newer, limit the version to Windows XP
#if(_MSC_VER < 1900) 
// Older Visual Studio
#define WINVER			0x0400      // Windows NT or higher
#define _WIN32_WINNT	0x0400		// Windows NT or higher
#else
// Visual Studio 2015 or better
#define WINVER			0x0501      // Windows XP or higher
#define _WIN32_WINNT	0x0501		// Windows XP or higher
#endif
#define _WIN32_WINDOWS	0x0410		// Windows 98 or higher
#define _WIN32_IE		0x0401		// Internet Explorer 4.01 or higher


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdisp.h>



#include "pragmas.h"
#include <PixeLINKApi.h>
#include <afxdisp.h>



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

