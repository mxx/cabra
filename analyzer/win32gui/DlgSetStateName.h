#if !defined(AFX_DLGSETSTATENAME_H__BF90AB4F_A4FB_4F42_8064_7F62BFC56A96__INCLUDED_)
#define AFX_DLGSETSTATENAME_H__BF90AB4F_A4FB_4F42_8064_7F62BFC56A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetStateName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetStateName dialog

class CDlgSetStateName : public CDialog
{
// Construction
public:
	CDlgSetStateName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetStateName)
	enum { IDD = IDD_DIALOG_STCONFIG };
	CString	m_strName1;
	CString	m_strName2;
	CString	m_strName3;
	CString	m_strName4;
	CString	m_strName5;
	CString	m_strName6;
	CString	m_strName7;
	CString	m_strName8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetStateName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetStateName)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETSTATENAME_H__BF90AB4F_A4FB_4F42_8064_7F62BFC56A96__INCLUDED_)
