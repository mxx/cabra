#if !defined(AFX_FILESETTINGDLG_H__53FD9593_D74A_4B3A_B5CC_38FE72EECE05__INCLUDED_)
#define AFX_FILESETTINGDLG_H__53FD9593_D74A_4B3A_B5CC_38FE72EECE05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileSettingDlg.h : header file
//
extern CAnalyzerApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CFileSettingDlg dialog

class CFileSettingDlg : public CDialog
{
// Construction
public:
	void InitComNo();
	void UpdateListItem(int MYNo);
	void InitListData();
	void InitListCtrl();
	CFileSettingDlg(CWnd* pParent = NULL);   // standard constructor
	DCB m_dcbConf;
	CString m_strDev;
// Dialog Data
	//{{AFX_DATA(CFileSettingDlg)
	enum { IDD = IDD_DIALOG_FILE_SETTING };
	CButton	m_btnConf;
	CSpinButtonCtrl	m_spinExt;
	CComboBoxEx	m_ComboComNo;
	CListCtrl	m_ListSet;
	CComboBox	m_ComboMyNo;
	CString	m_strFileName;
	int		m_DataCollectionByNo;
	CString	m_strExt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSettingDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CImageList m_listImage;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOpenFiledia();
	afx_msg void OnButtonSet();
	afx_msg void OnButtonDataCollection();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonComconf();
	afx_msg void OnDeltaposSpinExt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditExt();
	afx_msg void OnSelchangeComboMyno();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESETTINGDLG_H__53FD9593_D74A_4B3A_B5CC_38FE72EECE05__INCLUDED_)
