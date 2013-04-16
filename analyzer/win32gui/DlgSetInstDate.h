#if !defined(AFX_DLGSETINSTDATE_H__6D5E1863_BA21_4239_8BAA_0599EF0EC066__INCLUDED_)
#define AFX_DLGSETINSTDATE_H__6D5E1863_BA21_4239_8BAA_0599EF0EC066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetInstDate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetInstDate dialog

class CDlgSetInstDate : public CDialog
{
// Construction
public:
	CDlgSetInstDate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetInstDate)
	enum { IDD = IDD_DIALOG_INSTDATE };
	CTime	m_tInstDate;
	CTime	m_tInstTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetInstDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetInstDate)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETINSTDATE_H__6D5E1863_BA21_4239_8BAA_0599EF0EC066__INCLUDED_)
