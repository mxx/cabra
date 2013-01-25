// DataCollectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "DataCollectionDlg.h"
#include "SerialPort.h"
#include "Packet.h"
#include "DataFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT CommThreadProc( LPVOID pParam )
{
	//Open Port;
	CDataCollectionDlg& UI=*(CDataCollectionDlg*)pParam;;
	CPacket packet;
	CPacket send;
	do
	{
		if (packet.ReceiveFrame(UI.m_port)>0)
		{
			//UI.SetStatus(packet);
			UI.SendMessage(WM_UPDATE_DATA,(WPARAM)&packet,NULL);
			if (!UI.m_port.IsOpen()) return 0;
			switch(packet.GetPacketType())
			{
				case TYPE_GROUP:
					send.SendAck(UI.m_port);
					if (UI.NeedCollect(packet.GetMYNo()))
					{
						send.SendSCmd(UI.m_port);
						UI.SendMessage(WM_UPDATE_DATA,(WPARAM)&packet,1);						
					}
					else
					{
						send.SendNCmd(UI.m_port);
					}
				    break;
				case TYPE_TITLE:
				case TYPE_DATA:
				case TYPE_SPECTRUM:
					UI.SendMessage(WM_UPDATE_DATA,(WPARAM)&packet,1);
					send.SendAck(UI.m_port);
					break;
				default:
					if (packet.IsValid())
						send.SendAck(UI.m_port);
			}
		}
		
	}while(!UI.m_bStop);//(UI.m_hWnd && UI.m_port.IsOpen());
    UI.m_bStop = false;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg dialog


CDataCollectionDlg::CDataCollectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataCollectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataCollectionDlg)
	m_strFileName = _T("");
	m_strMYNo = _T("");
	m_strStartTime = _T("");
	m_strStatus = _T("Unreceived");
	m_strCurrentTime = _T("");
	//}}AFX_DATA_INIT
	pWorking = NULL;
	m_bStop = false;
}


void CDataCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataCollectionDlg)
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_strFileName);
	DDX_Text(pDX, IDC_STATIC_MYNO, m_strMYNo);
	DDX_Text(pDX, IDC_STATIC_STARTTIME, m_strStartTime);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_STATIC_CURRENTTIME, m_strCurrentTime);
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


void CDataCollectionDlg::SetStatus(CPacket &packet)
{
	if (packet.GetPacketType() == TYPE_GROUP)
	{
		m_strMYNo = packet.GetMYNo();
		m_strStatus = "Received";
		m_strStartTime = packet.GetDate() + " " + packet.GetTime();
	}

	if (packet.GetPacketType() == TYPE_UNKNOW
		|| packet.GetPacketType() == TYPE_END)
	{
		m_strMYNo.Empty();
		m_strStatus = "Unreceived";
		m_strStartTime.Empty();
		m_strFileName.Empty();
	}

	if (IsWindowVisible())	UpdateData(FALSE);

}

void CDataCollectionDlg::SaveData(CPacket& packet)
{
	CDataFile file;
	if (packet.GetPacketType() == TYPE_GROUP)
	{
		m_GroupPacket = packet;
		m_strMYNo = packet.GetMYNo();
		m_strFileName.Empty();

		if (NeedCollect(m_strMYNo))
		{
			int i = atoi(m_strMYNo);
			m_strFileName = g_SetArray[i].m_strFileName+"."+
					g_SetArray[i].m_Extension;
		}
		UpdateData(FALSE);
		return;
	}

	if (packet.GetPacketType() == TYPE_TITLE)
	{
		m_TitlePacket = packet;
		return;
	}

	if (packet.GetPacketType() == TYPE_DATA)
	{
		int i = atoi(m_strMYNo);
		
		if (NeedCollect(m_strMYNo))
		{
			m_strFileName = g_SetArray[i].m_strFileName+"."+
					g_SetArray[i].m_Extension;
			file.Save(m_strFileName,m_GroupPacket);
			file.Save(m_strFileName,m_TitlePacket);
			g_SetArray[i].m_Extension.Format("%.03d",
				atoi(g_SetArray[i].m_Extension)+1);
		}
		
	}

	if (NeedCollect(m_strMYNo))
		file.Save(m_strFileName,packet);
	UpdateData(FALSE);
}

void CDataCollectionDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent = 1)
	{
		CTime now = CTime::GetCurrentTime();
		m_strCurrentTime = now.Format("%Y/%m/%d %H:%M:%S");
		UpdateData(FALSE);
	}
	
	CDialog::OnTimer(nIDEvent);
}

bool CDataCollectionDlg::NeedCollect(CString &strMYNo)
{
		for(int i=1;i<13;i++)
		{
			if ((!strMYNo.IsEmpty()) && (!g_SetArray[i].m_strID.IsEmpty()) && atoi(g_SetArray[i].m_strID) == atoi(strMYNo))
			{
				return g_SetArray[i].m_DataCollection==TRUE;
			}
		}
		return false;
}

BOOL CDataCollectionDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CDataCollectionDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	CPacket& packet=*(CPacket*)wParam;
	SetStatus(packet);
	if (lParam) SaveData(packet);
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
