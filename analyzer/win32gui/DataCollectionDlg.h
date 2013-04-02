#if !defined(AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_)
#define AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_

#include "SerialPort.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataCollectionDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg dialog

class CDataCollectionDlg : public CDialog
{
// Construction
public:
	void ClosePort();
	CDataCollectionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataCollectionDlg)
	enum { IDD = IDD_DIALOG_DATA_COLLECTION };
	CEdit	m_ctlPrompt;
	CTabCtrl	m_tabComm;
	CString	m_strTime;
	CString	m_strUniqNo;
	CString	m_strVersion;
	CString	m_strStatus;
	//}}AFX_DATA
    DCB m_dcb;
	CSerialPort m_port;
	CString m_strDev;
	bool m_bStop;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataCollectionDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString strSending;
	CString strReceive;
	CWinThread *pWorking;
	void groupButtonSet(int first,int number);
    void showGETbuttons(int cmd);
    void showSETbuttons(int cmd);
    void showCHKbuttons(int cmd);
	// Generated message map functions
	//{{AFX_MSG(CDataCollectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFileSetting();
	afx_msg void OnButtonShutDown();
	afx_msg void OnButtonSpectrum();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnButtonVersion();
	afx_msg void OnSelchangeTabComm(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_)
