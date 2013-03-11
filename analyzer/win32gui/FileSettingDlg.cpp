// FileSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "FileSettingDlg.h"
#include "EnumSerial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileSettingDlg dialog


CFileSettingDlg::CFileSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileSettingDlg)

	

	//}}AFX_DATA_INIT
	memset(&m_dcbConf,0,sizeof(m_dcbConf));
	m_dcbConf.BaudRate = CBR_9600;
	m_dcbConf.ByteSize = 7;
	m_dcbConf.Parity = EVENPARITY;        // no parity bit
	m_dcbConf.StopBits =TWOSTOPBITS;    // one stop bit

}


void CFileSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileSettingDlg)
	DDX_Control(pDX, IDC_BUTTON_COMCONF, m_btnConf);
	DDX_Control(pDX, IDC_COMBO_COMNO, m_ComboComNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CFileSettingDlg)
	ON_BN_CLICKED(IDC_BUTTON_DATA_COLLECTION, OnButtonDataCollection)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_COMCONF, OnButtonComconf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileSettingDlg message handlers

BOOL CFileSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP1);
	m_listImage.Create(2,20,ILC_COLOR24|ILC_MASK,1,0);
	m_listImage.Add(&bmp,RGB(255,255,255));

	InitComNo();
	
	if (m_ComboComNo.GetCount() > 0)
	{
		m_ComboComNo.SetCurSel(0);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFileSettingDlg::PreTranslateMessage(MSG* pMsg) 
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



void CFileSettingDlg::OnButtonDataCollection() 
{
	int n;
	n = m_ComboComNo.GetCurSel();
	if (n == CB_ERR)
	{
		m_ComboComNo.GetEditCtrl()->GetWindowText(m_strDev);		
	}
	else
		m_ComboComNo.GetLBText(n,m_strDev);

	if (m_strDev.IsEmpty())
		AfxMessageBox("COM port not selected, Please select one, \nor the data can not be collected");

	::PostMessage(this->GetParent()->m_hWnd,WM_OPEN_DLG,OPEN_DATA_COLLECTION,OPEN_FILE_SETTING);
}

void CFileSettingDlg::InitComNo()
{
	CString strNo,strName;
	CArray<SSerInfo,SSerInfo&> asi;
	try
	{
		EnumSerialPorts(asi,TRUE);
		for (int i=0; i < asi.GetSize(); i++) 
		{
			strName = asi[i].strFriendlyName;
			int idx = strName.Find("(COM");
			if (idx > 0)
			{
				int last = strName.Find(")",idx+1);
				if (last > 0)
				{
					strNo = strName.Mid(idx + 1,last - idx - 1);
					COMBOBOXEXITEM item={0};
					item.mask = CBEIF_TEXT;
					item.pszText = (char*)(LPCTSTR)strNo;
					item.cchTextMax = strNo.GetLength();
					m_ComboComNo.InsertItem(&item);
				}
			}
			
		}
	}
	catch(CString strErr)
	{
		AfxMessageBox(strErr);
	}
}

void CFileSettingDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	UpdateData(FALSE);
}

void CFileSettingDlg::OnButtonComconf() 
{
	COMMCONFIG conf={0};
	conf.dwSize = sizeof(COMMCONFIG);
	conf.wVersion = 1;
	conf.dwProviderSize = 1;
	conf.dcb = m_dcbConf;
	int n;
	n = m_ComboComNo.GetCurSel();
	
	if (n == CB_ERR)
	{
		m_ComboComNo.GetEditCtrl()->GetWindowText(m_strDev);
	}
	else
		m_ComboComNo.GetLBText(n,m_strDev);

	if (CommConfigDialog(m_strDev,GetSafeHwnd(),&conf))
	{
		m_dcbConf = conf.dcb;
	}
	else
	{
		CErrorMsg msg;
		msg.GetErrorMsg("COM conf");
		AfxMessageBox(msg);
	}
}


