// DataCollectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxmt.h>
#include "Analyzer.h"
#include "VTDRVersion.h"
#include "VTDRRealTime.h"
#include "define_gbk.h"
#include "DataCollectionDlg.h"
#include "SerialPort.h"
#include "SeleteDate.h"
#include <string>
#include "protocol/Packet.h"
#include "protocol/Protocol.h"
#include "USBDataFilev2012.h"
#include "DlgSetVInfo.h"
#include "DlgSetInstDate.h"
#include "DlgSetOnPara.h"
#include "DlgSetStateName.h"
#include "USBFileLoading.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CString g_strUser;
#define D(x,y)  dict[#x] = _T( #y )
CMutex g_Mutex(FALSE,NULL);
using namespace std;
map<CString,LPCTSTR> CDataCollectionDlg::dict;


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

		if (n || strCache.size())
		{
            if (ptrUI->m_bDebug)
            {
                CString strDump;
                char tmp[12];
                strDump = "R: ";
                for(int i=0;i<n;i++)
                {
                    sprintf(tmp,"%02X ",0x000000FF & buf[i]);
                    strDump += tmp;
                }
                strDump += "\r\n";
                ptrUI->Prompt((LPCSTR)strDump);
            }
			strCache.append(buf, n);
            packet.Extract(strCache);
            CmdWord cmd = packet.GetCmd();
			if (cmd != CMD_OVER)
			{
				
				if (cmd == GET_ERROR || cmd == SET_ERROR || packet.GetContent().empty())
				{
					ptrUI->SendMessage(WM_UPDATE_DATA, NULL, cmd);
					continue;
				}

				int n  = pro.Parse(packet, ptrUI->m_Records);
				if (n)
				{
					ptrUI->SendMessage(WM_UPDATE_DATA, (WPARAM) n,
							NULL);
				}

			}
		}
		else
		{
			Sleep(5);
			ptrUI->SendMessage(WM_UPDATE_DATA, NULL, CMD_OVER);
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
	m_strUniqNo = _T("");
	m_strVersion = _T("");
	m_strStatus = _T("");
	m_bDebug = FALSE;
	m_bSaveComData = FALSE;
	m_strPlateNo = _T("");
	//}}AFX_DATA_INIT
	pWorking = NULL;
	m_bStop = false;
    m_bCheckMode = false;
    tStart = 0;
    tEnd = 0;
    nNum = 0;
    InitDict();
    USBDataFilev2012::initMap();
}

void CDataCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataCollectionDlg)
	DDX_Control(pDX, IDC_EDIT_PROMPT, m_ctlPrompt);
	DDX_Control(pDX, IDC_TAB_COMM, m_tabComm);
	DDX_Text(pDX, IDC_STATIC_UNIQNO, m_strUniqNo);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Check(pDX, IDC_CHECK_DEBUG, m_bDebug);
	DDX_Check(pDX, IDC_CHECK_SAVECOM, m_bSaveComData);
	DDX_Text(pDX, IDC_STATIC_PLATENO, m_strPlateNo);
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
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_VERSION, OnButtonVersion)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_COMM, OnSelchangeTabComm)
	ON_BN_CLICKED(IDC_BUTTON_DRIVER, OnButtonDriver)
	ON_BN_CLICKED(IDC_BUTTON_DRI, OnButtonDri)
	ON_BN_CLICKED(IDC_BUTTON_RTC, OnButtonRtc)
	ON_BN_CLICKED(IDC_BUTTON_OTD, OnButtonOtd)
	ON_BN_CLICKED(IDC_BUTTON_ACDR, OnButtonAcdr)
	ON_BN_CLICKED(IDC_BUTTON_CHKCLK, OnButtonChkclk)
	ON_BN_CLICKED(IDC_BUTTON_CHKPARA, OnButtonChkpara)
	ON_BN_CLICKED(IDC_BUTTON_CJKODR, OnButtonCjkodr)
	ON_BN_CLICKED(IDC_BUTTON_CLOCK, OnButtonClock)
	ON_BN_CLICKED(IDC_BUTTON_ENTCHECK, OnButtonEntcheck)
	ON_BN_CLICKED(IDC_BUTTON_EXITCHK, OnButtonExitchk)
	ON_BN_CLICKED(IDC_BUTTON_INITODR, OnButtonInitodr)
	ON_BN_CLICKED(IDC_BUTTON_INSTDATE, OnButtonInstdate)
	ON_BN_CLICKED(IDC_BUTTON_ODERMETER, OnButtonOdermeter)
	ON_BN_CLICKED(IDC_BUTTON_PARA, OnButtonPara)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, OnButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_POS, OnButtonPos)
	ON_BN_CLICKED(IDC_BUTTON_PWR, OnButtonPwr)
	ON_BN_CLICKED(IDC_BUTTON_SETCONF, OnButtonSetconf)
	ON_BN_CLICKED(IDC_BUTTON_SETPARA, OnButtonSetpara)
	ON_BN_CLICKED(IDC_BUTTON_SETVINFO, OnButtonSetvinfo)
	ON_BN_CLICKED(IDC_BUTTON_SPD, OnButtonSpd)
	ON_BN_CLICKED(IDC_BUTTON_STATCONF, OnButtonStatconf)
	ON_BN_CLICKED(IDC_BUTTON_STLOG, OnButtonStlog)
	ON_BN_CLICKED(IDC_BUTTON_UNIQNO, OnButtonUniqno)
	ON_BN_CLICKED(IDC_BUTTON_VINFO, OnButtonVinfo)
	ON_BN_CLICKED(IDC_CHECK_DEBUG, OnCheckDebug)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_UFILE, OnButtonUfile)
    ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_CHECK_SAVECOM, OnCheckSavecom)
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

    str.LoadString(IDS_DATA);
    m_tabComm.InsertItem(3,(LPCTSTR)str);

	strSending.LoadString(IDS_SENDING);
	strReceive.LoadString(IDS_RECEIVE);

	groupButtonSet(IDC_BUTTON_SETVINFO,6);
	groupButtonSet(IDC_BUTTON_ENTCHECK,5);

    CRect rect;
    CWnd* pWnd = GetDlgItem(IDC_STATIC_PROMPT);
    if (pWnd)
        pWnd->GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_tableData.Create(WS_CHILD|WS_BORDER|LVS_REPORT,rect, this, 1);

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
    HBRUSH brush_red = ::CreateSolidBrush(RGB(255,127,127) );
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
        else if (m_strStatus.Find(_T("����"))>=0)
        {
            pDC->SetBkColor(RGB(255,127,127) );
            return brush_red;
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
	int n = (int) wParam;
    
	if (n)
	{
        while(m_Records.size())
        {
            VTDRRecord* ptrRec = m_Records.front();
            m_Records.pop_front();
            string strDump;
            string strTitle;
            strTitle = _T("��");
            strTitle += USBDataFilev2012::DataBlockName[ptrRec->GetDataCode()];
            strTitle += _T("��\r\n");
            ptrRec->Dump(strDump);
            strDump += "\r\n";
            strTitle += strDump;
            Prompt(strTitle.c_str());
            
            m_strStatus.LoadString(IDS_RECEIVE);
            UpdateBaseInfo(ptrRec);

            delete ptrRec;
        };
	}
	else
	{
		CmdWord cmd = (CmdWord)lParam;

		if (cmd == GET_ERROR)
			m_strStatus.LoadString(IDS_GETERROR);
		else if (cmd == SET_ERROR)
			m_strStatus.LoadString(IDS_SETERROR);
		else if (cmd != CMD_OVER)
        {
            m_strStatus.LoadString(IDS_RECEIVE);
            CString str;
            str.Format("����%XӦ��\r\n",cmd & 0x0000FF);
            Prompt((LPCTSTR)str);
        }
        else
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

bool CDataCollectionDlg::getDateSetting(void)
{
    SeleteDate dlg;
    dlg.m_nRec = nNum;
    if (tStart || tEnd)
    {
        dlg.m_timeStart = CTime(tStart);
        dlg.m_timeEnd = CTime(tEnd);
    }
    else
        dlg.m_timeStart = CTime(0);

    if (dlg.DoModal()==IDOK)
    {
        tStart = dlg.m_timeStart.GetTime();
        tEnd = dlg.m_timeEnd.GetTime();
        nNum = dlg.m_nRec;
        return true;
    }
    return false;
}

void CDataCollectionDlg::sendCmd(CmdWord cmd,VTDRRecord* ptrRec)
{
    Protocol pro;
	Packet packet;
    packet = pro.Command(cmd,ptrRec);
	m_strStatus.LoadString(IDS_SENDING);
	UpdateData(FALSE);
    g_Mutex.Lock();
	m_port.Write(packet.GetData().data(), packet.GetData().size());
    g_Mutex.Unlock();
    Sleep(500);
	m_strStatus = "";
	UpdateData(FALSE);
}

void CDataCollectionDlg::sendCmd(CmdWord cmd, time_t tStart, time_t tEnd, int size)
{
	Protocol pro;
	Packet packet = pro.Command(cmd, tStart, tEnd, size);
	m_strStatus.LoadString(IDS_SENDING);
	UpdateData(FALSE);
    g_Mutex.Lock();
	m_port.Write(packet.GetData().data(), packet.GetData().size());
    g_Mutex.Unlock();
    Sleep(500);
	m_strStatus = "";
	UpdateData(FALSE);
}

void CDataCollectionDlg::showGETbuttons(int cmd)
{
    CWnd* pWnd;
    for(int i=0;i < 16 ;i++)
    {
        pWnd = GetDlgItem(IDC_BUTTON_VERSION+i);
        if (pWnd)
            pWnd->ShowWindow(cmd);   
    }
    pWnd = GetDlgItem(IDC_CHECK_SAVECOM);
    if (pWnd)
            pWnd->ShowWindow(cmd); 
    pWnd = GetDlgItem(IDC_BUTTON_UFILE);
    if (pWnd)
            pWnd->ShowWindow(cmd); 
}

void CDataCollectionDlg::showSETbuttons(int cmd)
{
    for(int i=0;i < 6 ;i++)
    {
        CWnd* pWnd = GetDlgItem(IDC_BUTTON_SETVINFO+i);
        if (pWnd)
        {
            pWnd->ShowWindow(cmd);
            pWnd->EnableWindow(g_strUser == "admin");
        }
    }

}

void CDataCollectionDlg::showCHKbuttons(int cmd)
{
    for(int i=0;i < 5 ;i++)
    {
        CWnd* pWnd = GetDlgItem(IDC_BUTTON_ENTCHECK+i);
        if (pWnd)
        {
            pWnd->ShowWindow(cmd);
            if (i)
                pWnd->EnableWindow(FALSE);
            else
                pWnd->SetFocus();
        }
    }
}

void CDataCollectionDlg::OnSelchangeTabComm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	switch (m_tabComm.GetCurFocus())
    {
    case 0:
        m_bCheckMode = false;
	    showGETbuttons(SW_SHOW);
	    showSETbuttons(SW_HIDE);
	    showCHKbuttons(SW_HIDE);
        m_tableData.ShowWindow(SW_HIDE);
        break;
    case 1:

        showSETbuttons(SW_SHOW);
        m_bCheckMode = false;
    	
        showCHKbuttons(SW_HIDE);
        showGETbuttons(SW_HIDE);
        m_tableData.ShowWindow(SW_HIDE);
        break;
    case 2:
        showGETbuttons(SW_HIDE);
        showSETbuttons(SW_HIDE);
        showCHKbuttons(SW_SHOW);
        m_tableData.ShowWindow(SW_HIDE);
        break;
    case 3:
        showGETbuttons(SW_HIDE);
        showSETbuttons(SW_HIDE);
        showCHKbuttons(SW_HIDE);
        m_tableData.ShowWindow(SW_SHOW);
        break;

    }
	*pResult = 0;
}

void CDataCollectionDlg::OnButtonVersion()
{
	sendCmd(GET_STD_VERSION, 0, 0, 0);
}

void CDataCollectionDlg::OnButtonDriver() 
{
	sendCmd(GET_Driver_LicenceID, 0, 0, 0);	
}

void CDataCollectionDlg::OnButtonDri() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Driver_Record,tStart,tEnd,nNum);	
    }	

}

void CDataCollectionDlg::OnButtonRtc() 
{
	sendCmd(GET_RealTime,0,0,0);
}

void CDataCollectionDlg::OnButtonOtd() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_OverDrive_Record,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonAcdr() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Suspious_Record,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonChkclk() 
{
	sendCmd(CHECK_Output_RTC_Pulse,0,0,0);
}

void CDataCollectionDlg::OnButtonChkpara() 
{
	sendCmd(CHECK_Output_Speed_Pulse,0,0,0);
}

void CDataCollectionDlg::OnButtonCjkodr() 
{
	sendCmd(CHECK_Input_Speed_Pulse,0,0,0);	
}

void CDataCollectionDlg::OnButtonClock() 
{
	CDlgSetInstDate dlg;
    dlg.m_tInstDate = CTime::GetCurrentTime();
    if (dlg.DoModal()==IDOK)
    {
        VTDRRealTime realTime;
        realTime.tTime = dlg.m_tInstDate.GetTime();
        sendCmd(SET_Clock,&realTime);
    }
}

void CDataCollectionDlg::OnButtonEntcheck() 
{
    
	sendCmd(CHECK_Enter,0,0,0);
    m_bCheckMode = true;
    enableCHKButton(TRUE);
    SetTimer(1,2000,NULL);
}

void CDataCollectionDlg::OnButtonExitchk() 
{
    m_bCheckMode = false;
	sendCmd(CHECK_Leave,0,0,0);
    KillTimer(1);
    enableCHKButton(FALSE);
}

void CDataCollectionDlg::OnButtonInitodr() 
{
	CDlgSetOnPara dlg;
    dlg.m_strTitle.LoadString(IDS_ODERMETERSTART);
    if (dlg.DoModal()==IDOK)
    {
        VTDROderMeterInit recd;
        recd.startMeter = (double)dlg.m_nPara/10.0;
        sendCmd(SET_Odometer,&recd);
    }
}

void CDataCollectionDlg::OnButtonInstdate() 
{
	CDlgSetInstDate dlg;
    dlg.m_tInstDate = CTime::GetCurrentTime();
    if (dlg.DoModal()==IDOK)
    {
        VTDRRealTime realTime;
        realTime.tTime = dlg.m_tInstDate.GetTime();
        sendCmd(SET_Install_Date,&realTime);
    }
}

void CDataCollectionDlg::OnButtonOdermeter() 
{
	sendCmd(GET_Odometer,0,0,0);	
}

void CDataCollectionDlg::OnButtonPara() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Param_Record,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonPlus() 
{
	sendCmd(GET_Pulse_Param,0,0,0);
}

void CDataCollectionDlg::OnButtonPos() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Postion_Record,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonPwr() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Power_Record,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonSetconf() 
{
	CDlgSetStateName dlg;
    if (dlg.DoModal()==IDOK)
    {
        VTDRVehicleConfigureSetup conf;
        conf.strNameOf[0] = (LPCTSTR)dlg.m_strName1;
        conf.strNameOf[1] = (LPCTSTR)dlg.m_strName2;
        conf.strNameOf[2] = (LPCTSTR)dlg.m_strName3;
        conf.strNameOf[3] = (LPCTSTR)dlg.m_strName4;
        conf.strNameOf[4] = (LPCTSTR)dlg.m_strName5;
        conf.strNameOf[5] = (LPCTSTR)dlg.m_strName6;
        conf.strNameOf[6] = (LPCTSTR)dlg.m_strName7;
        conf.strNameOf[7] = (LPCTSTR)dlg.m_strName8;
        sendCmd(SET_Param_Config,&conf);
    }
}

void CDataCollectionDlg::OnButtonSetpara() 
{
    CDlgSetOnPara dlg;
    dlg.m_strTitle.LoadString(IDS_PULSPARA);
    if (dlg.DoModal()==IDOK)
    {
        VTDRPulseModulus recd;
        recd.sModulus = dlg.m_nPara;
        recd.tTime = time(NULL);
        sendCmd(SET_Pulse_Param,&recd);
    }
}

void CDataCollectionDlg::OnButtonSetvinfo() 
{
	CDlgSetVInfo dlg;
    if (dlg.DoModal()==IDOK)
    {
        VTDRVehicleInfo info;
        info.strPlateClass = (LPCTSTR)dlg.m_strPClass;
        info.strPlateNumber = (LPCTSTR)dlg.m_strPNo;
        info.strTypeCode = (LPCTSTR)dlg.m_strVID;
        sendCmd(SET_Vehicle_Info,&info);      
    }
}

void CDataCollectionDlg::OnButtonSpd() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Speed_Record,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonStatconf() 
{
	sendCmd(GET_State_Config,0,0,0);	
}

void CDataCollectionDlg::OnButtonStlog() 
{
    if (getDateSetting())
    {
	    sendCmd(GET_Speed_State_Log,tStart,tEnd,nNum);	
    }
}

void CDataCollectionDlg::OnButtonUniqno() 
{
	sendCmd(GET_Uniq_No,0,0,0);	
}

void CDataCollectionDlg::OnButtonVinfo() 
{
	sendCmd(GET_Vehicle_Info,0,0,0);
}

void CDataCollectionDlg::OnCheckDebug() 
{
    UpdateData();	
}

void CDataCollectionDlg::Prompt(LPCSTR szTxt)
{
    CString strPrompt;
    CString strN = szTxt;
    
    m_ctlPrompt.GetWindowText(strPrompt);
    strPrompt += Tanslate(strN);
    
    while (strPrompt.GetLength() > m_ctlPrompt.GetLimitText())
    {
        int n = strPrompt.Find("\r\n");
        if (n<0) break;
        strPrompt = strPrompt.Right(strPrompt.GetLength()- n -2);
    };
    m_ctlPrompt.SetWindowText((LPCTSTR)strPrompt);
    int n = strPrompt.GetLength();
    m_ctlPrompt.SetSel(n,n);
}

void CDataCollectionDlg::InitDict()
{
    dict[" License"] = _T(" ��ʻ֤��");
    dict["Year"]=_T("��׼���");
    dict["DataCode"]=_T("���ݴ���");
    dict["Modify"]=_T("�޸ĺ�");
    D(Time,��¼�ǵ�ǰʱ��);
    D(InstallTime,��װʱ��);
    D(StartMeter,��ʼ���);
    D(AccumulateMeter,�ۼ���ʻ���);
    D(param,����ϵ��);
    D(TypeCode,����ʶ�����);
    D(PlateNo.,������������);
    D(PlateClass,���������Ʒ���);
    D(StateByte,״̬�ź��ֽ�);
    D(CCC-Code,������CCC��֤����);
    D(AuthrizedType,��֤�ͺ�);
    D(ProductDate,��������);
    D(SN,��Ʒ������ˮ��);
    D(StartTime,��ʼʱ��);
    D(SEC,��);
    D(SPEED,�ٶ�);
    D(STATE,״̬);
    D(minute,����);
    D(Speed,�ٶ�);
    D(Latitude,γ��);
    D(Longitude,����);
    D(Altitude,����);
    D(DriverLicense,��ʻ֤��);
    D(LastEndTime,��ʻ����ʱ��);
    D(Position,λ������);
    D(State,״̬�ź�);
    D(SECBEFORE,ͣ��ǰ����);
    D(startTime,��ʼʱ��);
    D(endTime,����ʱ��);
    D(RecTime,��¼ʱ��);
    D(RecType,��¼����);
    D(Unknown,δ֪);
    D(Login,��¼);
    D(Logout,�˳�);
    D(PowerOn,�ϵ�);
    D(PowerOff,����);
    dict["\n"]=_T("\r\n");

}

LPCTSTR CDataCollectionDlg::Tanslate(CString &str)
{
    map<CString,LPCTSTR>::iterator it;
    for(it = dict.begin();it != dict.end(); it++)
        str.Replace(it->first,it->second);
    return str;
}


void CDataCollectionDlg::CheckModeHeartbeat()
{
    if (!m_bCheckMode)
    {
        KillTimer(1);
        return;
    }
    sendCmd(CHECK_Enter,0,0,0);
}

void CDataCollectionDlg::enableCHKButton(BOOL enable)
{
    for(int i=1;i < 5 ;i++)
    {
        CWnd* pWnd = GetDlgItem(IDC_BUTTON_ENTCHECK+i);
        if (pWnd)
        {
            pWnd->EnableWindow(enable);
        }
    }
    if (!enable)
        GetDlgItem(IDC_BUTTON_ENTCHECK)->SetFocus();
}

void CDataCollectionDlg::OnTimer(UINT nIDEvent) 
{
    if (m_bCheckMode)
        CheckModeHeartbeat();

	CDialog::OnTimer(nIDEvent);
}

void CDataCollectionDlg::OnButtonUfile() 
{
    const char szFilter[] = _T("��¼������(*.VDR)|D*.VDR|All Files (*.*)|*.*||");

    CFileDialog dlg(TRUE,".VDR",NULL,OFN_ALLOWMULTISELECT,szFilter,this);
    if (dlg.DoModal()==IDOK)
    {
        CUSBFileLoading* pDlg = new CUSBFileLoading(this);
        CUSBFileLoading& dlgLoad = *pDlg;
        dlgLoad.m_pSelf = pDlg;
        POSITION pos = dlg.GetStartPosition();
        while(pos)
        {
            CString str = dlg.GetNextPathName(pos);
            dlgLoad.m_listFiles.push_back(str);  
        };
        dlgLoad.Create(dlgLoad.IDD,this);
        dlgLoad.ShowWindow(SW_SHOW);
    }	
}

void CDataCollectionDlg::OnCheckSavecom() 
{
	UpdateData();	
}

void CDataCollectionDlg::UpdateBaseInfo(VTDRRecord *ptrRec)
{
    if (!ptrRec) return;
    
    switch(ptrRec->GetDataCode())
    {
    case VTDRRecord::Version:
        {
            VTDRVersion* p = (VTDRVersion*)ptrRec;
            m_strVersion.Format("%d.%d",p->year,p->modify);
        }
        break;
    case VTDRRecord::VehicleInfo:
        {
            VTDRVehicleInfo* p = (VTDRVehicleInfo*)ptrRec;
            m_strPlateNo = p->strPlateNumber.c_str();
        }
        break;
    case VTDRRecord::UniqCode:
        {
            VTDRUniqCode* p = (VTDRUniqCode*)ptrRec;
            m_strUniqNo.Format("%s-%s-%02d%02d%02d-%d",p->strManufacture.c_str(),
                p->strManufacture.c_str(),
                p->nYear,
                p->nMonth,
                p->nDay,
                p->nSerialNumber);
        }
        break;
    };
    
}
