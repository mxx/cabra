// DataCollectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "VTDRVersion.h"
#include "DataCollectionDlg.h"
#include "SerialPort.h"
#include <string>
#include "protocol/Packet.h"
#include "protocol/Protocol.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

UINT CommThreadProc(LPVOID pParam)
{
	CDataCollectionDlg* ptrUI = (CDataCollectionDlg*) pParam;
	Packet packet;
	Protocol pro;
	string strCache;
	do
	{
		char buf[2048] =
		{ 0 };
		int n = 0;
		n = ptrUI->m_port.Read(buf, 2048);

		if (n)
		{
			strCache.append(buf, n);
			if (packet.Extract(strCache).size())
			{
				VTDRRecord* ptrRecord = pro.Parse(packet);
				if (ptrRecord)
				{
					ptrUI->SendMessage(WM_UPDATE_DATA, (WPARAM) ptrRecord,
							NULL);
				}
			}
		}
		else
		{
			Sleep(5);
			ptrUI->SendMessage(WM_UPDATE_DATA, NULL, NULL);
		}
	} while (!ptrUI->m_bStop);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg dialog

CDataCollectionDlg::CDataCollectionDlg(CWnd* pParent /*=NULL*/) :
		CDialog(CDataCollectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataCollectionDlg)
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
	DDX_Control(pDX, IDC_EDIT_PROMPT, m_ctlPrompt);
	DDX_Control(pDX, IDC_TAB_COMM, m_tabComm);
	DDX_Text(pDX, IDC_STATIC_TIME, m_strTime);
	DDX_Text(pDX, IDC_STATIC_UNIQNO, m_strUniqNo);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}

void CDataCollectionDlg::groupButtonSet(int first,int number)
{
	CRect rect,itemRect;
	m_tabComm.GetWindowRect(&rect);
	ScreenToClient(&rect);
	
	for(int i=0;i<number;i++)
    {
		CWnd* pWnd = GetDlgItem(first+i);
        pWnd->GetWindowRect(&itemRect);
		
        pWnd->MoveWindow(rect.CenterPoint().x - itemRect.Width()/2,
            rect.top + (i+2)*itemRect.Height(),
            itemRect.Width(),itemRect.Height());
		pWnd->ShowWindow(SW_HIDE);
		pWnd->BringWindowToTop();
    }
}

    BEGIN_MESSAGE_MAP(CDataCollectionDlg, CDialog)
    //{{AFX_MSG_MAP(CDataCollectionDlg)
    ON_BN_CLICKED(IDC_BUTTON_FILE_SETTING, OnButtonFileSetting)
    ON_BN_CLICKED(IDC_BUTTON_SHUT_DOWN, OnButtonShutDown)
    ON_BN_CLICKED(IDC_BUTTON_SPECTRUM, OnButtonSpectrum)
    ON_WM_CTLCOLOR()
    ON_WM_SHOWWINDOW()
    ON_WM_COPYDATA()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_VERSION, OnButtonVersion)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_COMM, OnSelchangeTabComm)
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_DATA,OnUpdateData)
    END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataCollectionDlg message handlers

BOOL CDataCollectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    CString str;
    
    str.LoadString(IDS_READTAB);
	m_tabComm.InsertItem(0, (LPCTSTR)str);

    str.LoadString(IDS_SETTAB);
    m_tabComm.InsertItem(1, (LPCTSTR)str);

    str.LoadString(IDS_CHECKTAB);
    m_tabComm.InsertItem(2, (LPCTSTR)str);

	strSending.LoadString(IDS_SENDING);
	strReceive.LoadString(IDS_RECEIVE);

	groupButtonSet(IDC_BUTTON_SETVINFO,6);
	groupButtonSet(IDC_BUTTON_ENTCHECK,5);
    return TRUE;  // return TRUE unless you set the focus to a control
	
}

BOOL CDataCollectionDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
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
	::PostMessage(this->GetParent()->m_hWnd, WM_OPEN_DLG, OPEN_FILE_SETTING, 0);
}

void CDataCollectionDlg::OnButtonShutDown()
{
	GetParent()->PostMessage(WM_CLOSE);
}

void CDataCollectionDlg::OnButtonSpectrum()
{
	::PostMessage(this->GetParent()->m_hWnd, WM_OPEN_DLG, OPEN_SPECTRUM, 0);
}

HBRUSH CDataCollectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	HBRUSH brush_green = ::CreateSolidBrush(RGB(0,255,0) );
	HBRUSH brush_blue = ::CreateSolidBrush(RGB(0,255,255) );
	LOGBRUSH log =
	{ 0 };
	CString str;
	int nID = pWnd->GetDlgCtrlID();
	if (nID == IDC_STATIC_STATUS)
	{
		if (m_strStatus == strReceive)
		{
			pDC->SetBkColor(RGB(0,255,0) );
			return brush_green;
		}
		else if (m_strStatus == strSending)
		{
			pDC->SetBkColor(RGB(0,255,255) );
			return brush_blue;
		}
		else
		{
			::GetObject(hbr, sizeof(log), &log);
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
		if (m_port.Open(m_strDev) < 0)
		{
			MessageBox(
					"Failed to open COM port " + m_strDev + "\n"
							+ m_port.m_strErr
							+ "Please check you COM port and settings in File Setting.",
					"Error", MB_ICONERROR);
			m_strDev.Empty();
			return;
		}
		pWorking = AfxBeginThread(CommThreadProc, this);
	}
}


LRESULT CDataCollectionDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	VTDRRecord* ptrRec = (VTDRRecord*) wParam;

	if (ptrRec)
	{
		string strDump;
		CString strPrompt;
		ptrRec->Dump(strDump);
		m_ctlPrompt.GetWindowText(strPrompt);
		strPrompt += strDump.c_str();
        strPrompt += "\r\n";
        while (strPrompt.GetLength() > m_ctlPrompt.GetLimitText())
        {
        	strPrompt = strPrompt.Right(strPrompt.GetLength()-m_ctlPrompt.LineLength(0));
        };
        m_ctlPrompt.SetWindowText((LPCTSTR)strPrompt);
        int n = strPrompt.GetLength();
        m_ctlPrompt.SetSel(n,n);
		m_strStatus.LoadString(IDS_RECEIVE);
        if (ptrRec->GetDataCode() == VTDRRecord::Version)
        {
            VTDRVersion* p = (VTDRVersion*)ptrRec;
            m_strVersion.Format("%d.%d",p->year,p->modify);
             
        }
		delete ptrRec;
	}
	else
	{
		m_strStatus.LoadString(IDS_WAITING);
	}
	UpdateData(FALSE);
	return 0;
}

void CDataCollectionDlg::ClosePort()
{
	if (pWorking)
	{
		CWaitCursor wait;
		m_port.Close();
		pWorking = NULL;
		int i = 10;
		while (m_bStop && i--)
			Sleep(1);
	}
}

void CDataCollectionDlg::OnClose()
{
    
	CDialog::OnClose();
}

void CDataCollectionDlg::OnDestroy()
{
 
 
	CDialog::OnDestroy();
	ClosePort();
}

void CDataCollectionDlg::OnButtonVersion()
{
	Protocol pro;
	Packet packet = pro.Command(GET_STD_VERSION, 0, 0, 0);
	m_strStatus.LoadString(IDS_SENDING);
	UpdateData(FALSE);
	m_port.Write(packet.GetData().data(), packet.GetData().size());
	m_strStatus = "";
	UpdateData(FALSE);
}

void CDataCollectionDlg::showGETbuttons(int cmd)
{
    for(int i=0;i < 16 ;i++)
    {
        CWnd* pWnd = GetDlgItem(IDC_BUTTON_VERSION+i);
        if (pWnd)
            pWnd->ShowWindow(cmd);   
    }
}

void CDataCollectionDlg::showSETbuttons(int cmd)
{
    for(int i=0;i < 6 ;i++)
    {
        CWnd* pWnd = GetDlgItem(IDC_BUTTON_SETVINFO+i);
        if (pWnd)
            pWnd->ShowWindow(cmd);
    }

}

void CDataCollectionDlg::showCHKbuttons(int cmd)
{
    for(int i=0;i < 5 ;i++)
    {
        CWnd* pWnd = GetDlgItem(IDC_BUTTON_ENTCHECK+i);
        if (pWnd)
            pWnd->ShowWindow(cmd);
    }
}

void CDataCollectionDlg::OnSelchangeTabComm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	switch (m_tabComm.GetCurFocus())
    {
    case 0:
	    showGETbuttons(SW_SHOW);
	    showSETbuttons(SW_HIDE);
	    showCHKbuttons(SW_HIDE);
        break;
    case 1:
    	showSETbuttons(SW_SHOW);
        showCHKbuttons(SW_HIDE);
        showGETbuttons(SW_HIDE);
        break;
    case 2:
        showGETbuttons(SW_HIDE);
        showSETbuttons(SW_HIDE);
        showCHKbuttons(SW_SHOW);
        break;
    }
	*pResult = 0;
}
