// DataCollectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "DataCollectionDlg.h"
#include "SerialPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT CommThreadProc( LPVOID pParam )
{
	//Open Port;

   
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg dialog


CDataCollectionDlg::CDataCollectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataCollectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataCollectionDlg)
	m_strPrompt = _T("");
	m_strTime = _T("");
	m_strUniqNo = _T("");
	m_strVersion = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
	pWorking = NULL;
	m_bStop = false;
}


void CDataCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataCollectionDlg)
	DDX_Text(pDX, IDC_EDIT_PROMPT, m_strPrompt);
	DDX_Text(pDX, IDC_STATIC_TIME, m_strTime);
	DDX_Text(pDX, IDC_STATIC_UNIQNO, m_strUniqNo);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataCollectionDlg, CDialog)
	//{{AFX_MSG_MAP(CDataCollectionDlg)
	ON_BN_CLICKED(IDC_BUTTON_FILE_SETTING, OnButtonFileSetting)
	ON_BN_CLICKED(IDC_BUTTON_AWS_FACTOR, OnButtonAwsFactor)
	ON_BN_CLICKED(IDC_BUTTON_SHUT_DOWN, OnButtonShutDown)
	ON_BN_CLICKED(IDC_BUTTON_SPECTRUM, OnButtonSpectrum)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_COPYDATA()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_VERSION, OnButtonVersion)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_DATA,OnUpdateData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg message handlers

BOOL CDataCollectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDataCollectionDlg::PreTranslateMessage(MSG* pMsg) 
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

void CDataCollectionDlg::OnButtonFileSetting() 
{
	::PostMessage(this->GetParent()->m_hWnd,WM_OPEN_DLG,OPEN_FILE_SETTING,0);
}

void CDataCollectionDlg::OnButtonAwsFactor() 
{

	::PostMessage(this->GetParent()->m_hWnd,WM_OPEN_DLG,OPEN_AWS_FACTOR,0);
}

void CDataCollectionDlg::OnButtonShutDown() 
{
	GetParent()->PostMessage(WM_CLOSE);
}

void CDataCollectionDlg::OnButtonSpectrum() 
{
	::PostMessage(this->GetParent()->m_hWnd,WM_OPEN_DLG,OPEN_SPECTRUM,0);
}

HBRUSH CDataCollectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	HBRUSH brush_green = ::CreateSolidBrush(RGB(0,255,0));
    LOGBRUSH log={0};
	int nID = pWnd->GetDlgCtrlID();
	if (nID == IDC_STATIC_STATUS)
	{
		if (m_strStatus == "Received")
		{
			pDC->SetBkColor(RGB(0,255,0));
			return brush_green;
		}
		else
		{
			::GetObject(hbr,sizeof(log),&log);
			pDC->SetBkColor(log.lbColor);
		}
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDataCollectionDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if (bShow && !m_strDev.IsEmpty() && !pWorking)
	{
		m_port.m_dcb.BaudRate = m_dcb.BaudRate;
		if (m_port.Open(m_strDev)<0)
		{
			MessageBox("Failed to open COM port " +m_strDev + "\n"+
				m_port.m_strErr + "Please check you COM port and settings in File Setting.",
				"Error",MB_ICONERROR);
			m_strDev.Empty();
			return;
		}
		pWorking = AfxBeginThread(CommThreadProc,this);
	}

	if (bShow && pWorking)
	{
		SetTimer(1,1000,0);
	}
	else
	{
		KillTimer(1);
	}

}


//DEL void CDataCollectionDlg::SetStatus(CString str)
//DEL {
//DEL 	if (packet.GetPacketType() == TYPE_GROUP)
//DEL 	{
//DEL 		
//DEL 		m_strStatus = "Received";
//DEL 	
//DEL 	}
//DEL 
//DEL 	if (packet.GetPacketType() == TYPE_UNKNOW
//DEL 		|| packet.GetPacketType() == TYPE_END)
//DEL 	{
//DEL 		
//DEL 		m_strStatus = "Unreceived";
//DEL 		
//DEL 		
//DEL 	}
//DEL 
//DEL 	if (IsWindowVisible())	UpdateData(FALSE);
//DEL 
//DEL }


void CDataCollectionDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent = 1)
	{
		CTime now = CTime::GetCurrentTime();
		
		UpdateData(FALSE);
	}
	
	CDialog::OnTimer(nIDEvent);
}



BOOL CDataCollectionDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CDataCollectionDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	//CPacket& packet=*(CPacket*)wParam;
	//SetStatus(packet);
	
	return 0;
}



void CDataCollectionDlg::ClosePort()
{
	if (pWorking)
	{
		CWaitCursor wait;
		m_port.Close();
		pWorking = NULL;
		int i=10;
		while(m_bStop&&i--) Sleep(1);
	}
}

void CDataCollectionDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void CDataCollectionDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	ClosePort();
}

void CDataCollectionDlg::OnButtonVersion() 
{
	
}
