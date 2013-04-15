// DataCollectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "VTDRVersion.h"
#include "define_gbk.h"
#include "DataCollectionDlg.h"
#include "SerialPort.h"
#include "SeleteDate.h"
#include <string>
#include "protocol/Packet.h"
#include "protocol/Protocol.h"
#include "USBDataFilev2012.h"
#include "DlgSetVInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define D(x,y)  dict[#x] = _T( #y )

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
	m_strTime = _T("");
	m_strUniqNo = _T("");
	m_strVersion = _T("");
	m_strStatus = _T("");
	m_bDebug = FALSE;
	//}}AFX_DATA_INIT
	pWorking = NULL;
	m_bStop = false;
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
	DDX_Text(pDX, IDC_STATIC_TIME, m_strTime);
	DDX_Text(pDX, IDC_STATIC_UNIQNO, m_strUniqNo);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Check(pDX, IDC_CHECK_DEBUG, m_bDebug);
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
    ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_CHECK_DEBUG, OnCheckDebug)
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
        else if (m_strStatus.Find(_T("错误"))>=0)
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
            strTitle = USBDataFilev2012::DataBlockName[ptrRec->GetDataCode()];
            strTitle += "\r\n";
            ptrRec->Dump(strDump);
            strDump += "\r\n";
            strTitle += strDump;
            Prompt(strTitle.c_str());
            
            m_strStatus.LoadString(IDS_RECEIVE);
            if (ptrRec->GetDataCode() == VTDRRecord::Version)
            {
                VTDRVersion* p = (VTDRVersion*)ptrRec;
                m_strVersion.Format("%d.%d",p->year,p->modify);
            }
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
            str.Format("命令%X应答\r\n",cmd & 0x0000FF);
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

void CDataCollectionDlg::sendCmd(VTDRRecord* ptrRec)
{
    Protocol pro;
	Packet packet;
    packet = pro.Command(ptrRec);
	m_strStatus.LoadString(IDS_SENDING);
	UpdateData(FALSE);
	m_port.Write(packet.GetData().data(), packet.GetData().size());
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
	m_port.Write(packet.GetData().data(), packet.GetData().size());
    Sleep(500);
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
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonChkpara() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonCjkodr() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonClock() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonEntcheck() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonExitchk() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonInitodr() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonInstdate() 
{
	// TODO: Add your control notification handler code here
	
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
	// TODO: Add your control notification handler code here
	
}

void CDataCollectionDlg::OnButtonSetpara() 
{

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
        sendCmd(&info);      
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
    dict[" License"] = _T(" 驾驶证号");
    dict["Year"]=_T("标准年号");
    dict["DataCode"]=_T("数据代码");
    dict["Modify"]=_T("修改号");
    D(Time,记录仪当前时间);
    D(InstallTime,安装时间);
    D(StartMeter,初始里程);
    D(AccumulateMeter,累计行驶里程);
    D(param,脉冲系数);
    D(TypeCode,车辆识别代号);
    D(PlateNo.,机动车号牌码);
    D(PlateClass,机动车号牌分类);
    D(StateByte,状态信号字节);
    D(CCC-Code,生产厂CCC认证代码);
    D(AuthrizedType,认证型号);
    D(ProductDate,生产日期);
    D(SN,产品生产流水号);
    D(StartTime,开始时间);
    D(SEC,秒);
    D(SPEED,速度);
    D(STATE,状态);
    D(minute,分钟);
    D(Speed,速度);
    D(Latitude,纬度);
    D(Longitude,经度);
    D(Altitude,海拔);
    D(DriverLicense,驾驶证号);
    D(LastEndTime,行驶结束时间);
    D(Position,位置坐标);
    D(State,状态信号);
    D(SECBEFORE,停车前秒数);
    D(startTime,开始时间);
    D(endTime,结束时间);
    D(RecTime,记录时间);
    D(RecType,记录类型);
    D(Unknown,未知);
    D(Login,登录);
    D(Logout,退出);
    D(PowerOn,上电);
    D(PowerOff,掉电);

}

LPCTSTR CDataCollectionDlg::Tanslate(CString &str)
{
    map<CString,LPCTSTR>::iterator it;
    for(it = dict.begin();it != dict.end(); it++)
        str.Replace(it->first,it->second);
    return str;
}

