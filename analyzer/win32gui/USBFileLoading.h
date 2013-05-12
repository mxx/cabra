#if !defined(AFX_USBFILELOADING_H__14FE2110_0DDB_4E26_989F_E7E07B3267EE__INCLUDED_)
#define AFX_USBFILELOADING_H__14FE2110_0DDB_4E26_989F_E7E07B3267EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// USBFileLoading.h : header file
//
#include <list>
#include "..\DATASTORE\DataStore.h"	// Added by ClassView
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CUSBFileLoading dialog

class CUSBFileLoading : public CDialog
{
// Construction
public:
	DataStore m_ds;
	void DoLoad(void);
	CUSBFileLoading(CWnd* pParent = NULL);   // standard constructor
    CUSBFileLoading* m_pSelf;
    list<CString> m_listFiles;
// Dialog Data
	//{{AFX_DATA(CUSBFileLoading)
	enum { IDD = IDD_DIALOG_USBFLOAD };
	CString	m_strProgress;
	CString	m_strMsg;
	//}}AFX_DATA

    BOOL m_bStop;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBFileLoading)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void loadData(CString& strFilePath);

	// Generated message map functions
	//{{AFX_MSG(CUSBFileLoading)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBFILELOADING_H__14FE2110_0DDB_4E26_989F_E7E07B3267EE__INCLUDED_)
