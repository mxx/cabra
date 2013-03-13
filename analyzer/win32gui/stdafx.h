// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B86455A3_450E_4115_B56D_4984A9E2E507__INCLUDED_)
#define AFX_STDAFX_H__B86455A3_450E_4115_B56D_4984A9E2E507__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "ErrorMsg.h"


#define OPEN_FILE_SETTING			1
#define OPEN_DATA_COLLECTION		2
#define OPEN_AWS_FACTOR				3
#define OPEN_AWS_FACTOR_SETTING		4
#define OPEN_SPECTRUM				5

#define WM_OPEN_DLG			WM_USER+200
#define WM_UPDATE_DATA		WM_USER+300
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B86455A3_450E_4115_B56D_4984A9E2E507__INCLUDED_)
