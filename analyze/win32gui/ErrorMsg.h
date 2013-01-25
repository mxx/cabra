// ErrorMsg.h: interface for the CErrorMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORMSG_H__A18A13A5_5975_4A85_942F_EA11AEAA19A6__INCLUDED_)
#define AFX_ERRORMSG_H__A18A13A5_5975_4A85_942F_EA11AEAA19A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CErrorMsg : public CString  
{
public:
	void GetErrorMsg(LPCTSTR szTitle);
	CErrorMsg();
	virtual ~CErrorMsg();
	int m_nErr;

};

#endif // !defined(AFX_ERRORMSG_H__A18A13A5_5975_4A85_942F_EA11AEAA19A6__INCLUDED_)
