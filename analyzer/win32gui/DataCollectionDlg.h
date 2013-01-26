#if !defined(AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_)
#define AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_

#include "SerialPort.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataCollectionDlg.h : header file
//
#include "Packet.h"
/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg dialog

class CDataCollectionDlg : public CDialog
{
// Construction
public:
	void ClosePort();
	bool NeedCollect(CString& strMYNo);
	void SaveData(CPacket& packet);
	void SetStatus(CPacket& packet);
	CDataCollectionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataCollectionDlg)
	enum { IDD = IDD_DIALOG_DATA_COLLECTION };
	CString	m_strFileName;
	CString	m_strMYNo;
	CString	m_strStartTime;
	CString	m_strStatus;
	CString	m_strCurrentTime;
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
	CPacket m_TitlePacket;
	CPacket m_GroupPacket;

	CString strCurrentFile;
	
	CWinThread *pWorking;

	// Generated message map functions
	//{{AFX_MSG(CDataCollectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFileSetting();
	afx_msg void OnButtonAwsFactor();
	afx_msg void OnButtonShutDown();
	afx_msg void OnButtonSpectrum();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_)
