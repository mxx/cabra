#if !defined(AFX_SELETEDATE_H__EE12D267_5430_4CB6_8ADE_93B3817EEF4A__INCLUDED_)
#define AFX_SELETEDATE_H__EE12D267_5430_4CB6_8ADE_93B3817EEF4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SeleteDate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SeleteDate dialog

class SeleteDate : public CDialog
{
// Construction
public:
	SeleteDate(CWnd* pParent = NULL);   // standard constructor
  
// Dialog Data
	//{{AFX_DATA(SeleteDate)
	enum { IDD = IDD_DIALOG_SENDPARA };
	CDateTimeCtrl	m_ctlStart;
	CDateTimeCtrl	m_ctlEnd;
	int		m_nRec;
	CTime	m_timeEnd;
	CTime	m_timeStart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SeleteDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SeleteDate)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELETEDATE_H__EE12D267_5430_4CB6_8ADE_93B3817EEF4A__INCLUDED_)
