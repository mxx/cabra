#if !defined(AFX_SELECTLIST_H__55092DB7_9F98_43C2_A413_192351CAF4EF__INCLUDED_)
#define AFX_SELECTLIST_H__55092DB7_9F98_43C2_A413_192351CAF4EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectList dialog

class CSelectList : public CDialog
{
// Construction
public:
	CString m_strTitle;
	int m_nSelIndex;
	CSelectList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectList)
	enum { IDD = IDD_DIALOG_LIST };
	CListBox	m_list;
	//}}AFX_DATA

	CString m_strList[5];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectList)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectList)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTLIST_H__55092DB7_9F98_43C2_A413_192351CAF4EF__INCLUDED_)
