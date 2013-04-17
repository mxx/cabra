#if !defined(AFX_DLGSETONPARA_H__0032B7AA_43F0_4A45_B50B_D60E26E4C795__INCLUDED_)
#define AFX_DLGSETONPARA_H__0032B7AA_43F0_4A45_B50B_D60E26E4C795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetOnPara.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetOnPara dialog

class CDlgSetOnPara : public CDialog
{
// Construction
public:
	CDlgSetOnPara(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetOnPara)
	enum { IDD = IDD_DIALOG_ONEPARA };
	UINT	m_nPara;
	CString	m_strTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetOnPara)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetOnPara)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETONPARA_H__0032B7AA_43F0_4A45_B50B_D60E26E4C795__INCLUDED_)
