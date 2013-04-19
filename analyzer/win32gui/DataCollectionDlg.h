#if !defined(AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_)
#define AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_

#include "SerialPort.h"	// Added by ClassView
#include "protocol/Packet.h"
#include <time.h>
#include <map>
#include <list>

using namespace std;
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataCollectionDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg dialog
class VTDRRecord;
class CDataCollectionDlg : public CDialog
{
// Construction
public:
	void CheckModeHeartbeat(void);
	static LPCTSTR Tanslate(CString& str);
	static void InitDict(void);
	void Prompt(LPCSTR szTxt);
	void ClosePort();
	CDataCollectionDlg(CWnd* pParent = NULL);   // standard constructor
    list<VTDRRecord*> m_Records;
// Dialog Data
	//{{AFX_DATA(CDataCollectionDlg)
	enum { IDD = IDD_DIALOG_DATA_COLLECTION };
	CEdit	m_ctlPrompt;
	CTabCtrl	m_tabComm;
	CString	m_strTime;
	CString	m_strUniqNo;
	CString	m_strVersion;
	CString	m_strStatus;
	BOOL	m_bDebug;
	//}}AFX_DATA
    DCB m_dcb;
	CSerialPort m_port;
	CString m_strDev;
	bool m_bStop;
    bool m_bCheckMode;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataCollectionDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	static map<CString,LPCTSTR> dict;
// Implementation
protected:
	void enableCHKButton(BOOL enable);
	void sendCmd(CmdWord cmd,VTDRRecord* ptrRec);

	CString strSending;
	CString strReceive;
	CWinThread *pWorking;
    time_t tStart;
    time_t tEnd;
    time_t tLastBeat;
    int nNum;
	void groupButtonSet(int first,int number);
    void showGETbuttons(int cmd);
    void showSETbuttons(int cmd);
    void showCHKbuttons(int cmd);
    void sendCmd(CmdWord cmd, time_t tStart, time_t tEnd, int size);
    bool getDateSetting(void);
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
	afx_msg void OnButtonDriver();
	afx_msg void OnButtonDri();
	afx_msg void OnButtonRtc();
	afx_msg void OnButtonOtd();
	afx_msg void OnButtonAcdr();
	afx_msg void OnButtonChkclk();
	afx_msg void OnButtonChkpara();
	afx_msg void OnButtonCjkodr();
	afx_msg void OnButtonClock();
	afx_msg void OnButtonEntcheck();
	afx_msg void OnButtonExitchk();
	afx_msg void OnButtonInitodr();
	afx_msg void OnButtonInstdate();
	afx_msg void OnButtonOdermeter();
	afx_msg void OnButtonPara();
	afx_msg void OnButtonPlus();
	afx_msg void OnButtonPos();
	afx_msg void OnButtonPwr();
	afx_msg void OnButtonSetconf();
	afx_msg void OnButtonSetpara();
	afx_msg void OnButtonSetvinfo();
	afx_msg void OnButtonSpd();
	afx_msg void OnButtonStatconf();
	afx_msg void OnButtonStlog();
	afx_msg void OnButtonUniqno();
	afx_msg void OnButtonVinfo();
	afx_msg void OnCheckDebug();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATACOLLECTIONDLG_H__A17B0148_F445_4D87_9680_B4D64B73D864__INCLUDED_)
