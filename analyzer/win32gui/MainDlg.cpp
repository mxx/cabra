// LSCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLSCDlg dialog

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLSCDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLSCDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	//{{AFX_MSG_MAP(CLSCDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OPEN_DLG,OnOpenDlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLSCDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	this->GetWindowRect(&m_IniRect);
	m_FileSettingDlg.Create(IDD_DIALOG_FILE_SETTING,this);
	m_FileSettingDlg.ShowWindow(SW_SHOW);
	//this->SetWindowText("File Setting");
	CRect rect;
	m_FileSettingDlg.GetClientRect(&rect);
	CRect rc,rc2;
	this->GetWindowRect(&rc);
	this->GetClientRect(&rc2);
	
	rc.bottom = rc.top + rect.bottom + (rc.Height() - rc2.Height());
	rc.right = rc.left + rect.right + (rc.Width() - rc2.Width());
	MoveWindow(&rc);
	CenterWindow();

	m_DataCollectionDlg.Create(IDD_DIALOG_DATA_COLLECTION,this);
	m_DataCollectionDlg.ShowWindow(SW_HIDE);
	
	
	m_SpectrumDlg.Create(IDD_DIALOG_SPECTRUM,this);
	m_SpectrumDlg.ShowWindow(SW_HIDE);

	m_DataCollectionDlg.EnableWindow(FALSE);
	
	m_SpectrumDlg.EnableWindow(FALSE);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
			break;
		case VK_ESCAPE:
			return TRUE;
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CMainDlg::OnOpenDlg(WPARAM wParam, LPARAM lParam)
{
	m_FileSettingDlg.ShowWindow(SW_HIDE);
	m_DataCollectionDlg.ShowWindow(SW_HIDE);
	m_SpectrumDlg.ShowWindow(SW_HIDE);

	m_FileSettingDlg.EnableWindow(FALSE);
	m_DataCollectionDlg.EnableWindow(FALSE);
	m_SpectrumDlg.EnableWindow(FALSE);
	
	
	CRect rect;
	switch (wParam)
	{
		case OPEN_FILE_SETTING:
			m_FileSettingDlg.ShowWindow(SW_SHOW);
			this->SetWindowText("串口设置");
			m_FileSettingDlg.m_btnConf.EnableWindow(m_DataCollectionDlg.m_bStop);
			m_FileSettingDlg.GetClientRect(&rect);
			m_FileSettingDlg.EnableWindow();
			m_FileSettingDlg.SetFocus();
			break;
		case OPEN_DATA_COLLECTION:
			m_DataCollectionDlg.m_dcb.BaudRate = m_FileSettingDlg.m_dcbConf.BaudRate;
			if (lParam == OPEN_FILE_SETTING)
				m_DataCollectionDlg.m_strDev = m_FileSettingDlg.m_strDev;
			m_DataCollectionDlg.ShowWindow(SW_SHOW);
			this->SetWindowText("数据采集");
			m_DataCollectionDlg.GetClientRect(&rect);
			m_DataCollectionDlg.EnableWindow();
			m_DataCollectionDlg.SetFocus();
			break;
		case OPEN_SPECTRUM:
			m_SpectrumDlg.ShowWindow(SW_SHOW);
			m_SpectrumDlg.UpdateData(FALSE);
			this->SetWindowText("疑点数据图示");
			m_SpectrumDlg.EnableWindow();
			m_SpectrumDlg.SetFocus();
			m_SpectrumDlg.GetClientRect(&rect);
			break;
		default:
			break;
	}
	CRect rc,rc2;
	this->GetWindowRect(&rc);
	this->GetClientRect(&rc2);

	rc.bottom = rc.top + rect.bottom + (rc.Height() - rc2.Height());
	rc.right = rc.left + rect.right + (rc.Width() - rc2.Width());
	MoveWindow(&rc);
	CenterWindow();
}

void CMainDlg::OnClose() 
{
	CString strTitle;
	
	GetWindowText(strTitle);
	if (strTitle != "数据采集" )
	{
		::PostMessage(m_hWnd,WM_OPEN_DLG,OPEN_DATA_COLLECTION,0);
		return;
	}

	if (MessageBox("确认要关闭程序?","关闭",MB_OKCANCEL) == IDOK)
	{
		m_DataCollectionDlg.m_bStop = true;
		m_DataCollectionDlg.ClosePort();

		CDialog::OnClose();
	}
}

void CMainDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	CString strSec="Setting";
	CString strEnt;

	
}
