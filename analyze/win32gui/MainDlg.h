// LSCDlg.h : header file
//

#if !defined(AFX_LSCDLG_H__072E23D8_E617_45B5_A224_9CCD52E31301__INCLUDED_)
#define AFX_LSCDLG_H__072E23D8_E617_45B5_A224_9CCD52E31301__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataCollectionDlg.h"
#include "FileSettingDlg.h"
#include "SpectrumDlg.h"
#include "AWSFactorDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CLSCDlg dialog



class CMainDlg : public CDialog
{
// Construction
public:
	void OnOpenDlg(WPARAM wParam, LPARAM lParam);
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLSCDlg)
	enum { IDD = IDD_LSC_DIALOG };

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLSCDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	CRect m_IniRect;
	CFileSettingDlg m_FileSettingDlg;
	CDataCollectionDlg m_DataCollectionDlg;
	CAWSFactorDlg m_AWSFactorDlg;
	CSpectrumDlg m_SpectrumDlg;
// Implementation
protected:

	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLSCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LSCDLG_H__072E23D8_E617_45B5_A224_9CCD52E31301__INCLUDED_)
