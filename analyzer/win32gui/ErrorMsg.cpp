// ErrorMsg.cpp: implementation of the CErrorMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Analyzer.h"
#include "ErrorMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CErrorMsg::CErrorMsg()
{
	
}

CErrorMsg::~CErrorMsg()
{

}

void CErrorMsg::GetErrorMsg(LPCTSTR szTitle)
{
	m_nErr = GetLastError();
	LPVOID lpMsgBuf;
	::FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		m_nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	if (szTitle) Format(_T("%s:%s"),szTitle,lpMsgBuf);
	else
		Format("%s",lpMsgBuf);
	// Free the buffer.
	LocalFree( lpMsgBuf );
}
