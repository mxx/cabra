#if !defined(AFX_DLGSETVINFO_H__74507831_E3B5_4ECA_B155_9B9439E09D63__INCLUDED_)
#define AFX_DLGSETVINFO_H__74507831_E3B5_4ECA_B155_9B9439E09D63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetVInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetVInfo dialog

class CDlgSetVInfo : public CDialog
{
// Construction
public:
	CDlgSetVInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetVInfo)
	enum { IDD = IDD_DIALOG_SETVINFO };
	CString	m_strPClass;
	CString	m_strPNo;
	CString	m_strVID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetVInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetVInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETVINFO_H__74507831_E3B5_4ECA_B155_9B9439E09D63__INCLUDED_)
