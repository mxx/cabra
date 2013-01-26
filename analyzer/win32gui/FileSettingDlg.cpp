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
	m_strFileName = _T("");
	m_DataCollectionByNo = 1;
	m_strExt = _T("");
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
	DDX_Control(pDX, IDC_SPIN_EXT, m_spinExt);
	DDX_Control(pDX, IDC_COMBO_COMNO, m_ComboComNo);
	DDX_Control(pDX, IDC_LIST_SET, m_ListSet);
	DDX_Control(pDX, IDC_COMBO_MYNO, m_ComboMyNo);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Radio(pDX, IDC_RADIO_COLLECTION_YES, m_DataCollectionByNo);
	DDX_Text(pDX, IDC_EDIT_EXT, m_strExt);
	DDV_MaxChars(pDX, m_strExt, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CFileSettingDlg)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILEDIA, OnButtonOpenFiledia)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_DATA_COLLECTION, OnButtonDataCollection)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_COMCONF, OnButtonComconf)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXT, OnDeltaposSpinExt)
	ON_EN_CHANGE(IDC_EDIT_EXT, OnChangeEditExt)
	ON_CBN_SELCHANGE(IDC_COMBO_MYNO, OnSelchangeComboMyno)
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
	m_ListSet.SetImageList(&m_listImage, LVSIL_SMALL);
	InitListCtrl();
	InitListData();
	InitComNo();
	
	m_spinExt.SetRange(0,999);
	m_spinExt.SetPos(0);
	m_strExt.Format("%.03d",(short)m_spinExt.GetPos());

	if (m_ComboComNo.GetCount() > 0)
	{
		m_ComboComNo.SetCurSel(0);
	}
	m_ComboMyNo.SetCurSel(0);

	OnSelchangeComboMyno();
	
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

void CFileSettingDlg::OnButtonOpenFiledia() 
{
	UpdateData(TRUE);
	CString strFileName,strFName;
    
	CFileDialog FDlg(FALSE,m_strExt,"",0,
		"All File(*.*)|*.*||",NULL);
	if (FDlg.DoModal() == IDOK)
	{
		strFileName = FDlg.GetPathName();
		
		strFName = strFileName.SpanExcluding(".");
		m_strFileName = strFName;
		UpdateData(FALSE);
	}
	else
	{
		return;
	}
	
}

void CFileSettingDlg::InitListCtrl()
{
	int nWidth = 35;
	CRect rect;
	m_ListSet.GetClientRect(&rect);
	if (rect.Width()) nWidth=rect.Width()/17;

	LV_COLUMN lvcColumn;
	lvcColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcColumn.fmt  = LVCFMT_LEFT;
	
	lvcColumn.pszText	= "MY No.";
	lvcColumn.iSubItem	= 0;
	lvcColumn.cx		= 2*nWidth;
	m_ListSet.InsertColumn(0, &lvcColumn );
	
	lvcColumn.fmt  = LVCFMT_CENTER;
	lvcColumn.pszText	= "Data collection";
	lvcColumn.iSubItem	= 1;
	lvcColumn.cx		= 3*nWidth;
	m_ListSet.InsertColumn(1, &lvcColumn );
	
	lvcColumn.fmt  = LVCFMT_LEFT;
	lvcColumn.pszText	= "File Name";
	lvcColumn.iSubItem	= 4;
	lvcColumn.cx		= 10*nWidth;
	m_ListSet.InsertColumn(2, &lvcColumn );
	
	lvcColumn.fmt  = LVCFMT_CENTER;
	lvcColumn.pszText	= "Extension";
	lvcColumn.iSubItem	= 2;
	lvcColumn.cx		= 2*nWidth;
	m_ListSet.InsertColumn(3, &lvcColumn );
	
	m_ListSet.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
}

void CFileSettingDlg::InitListData()
{
	CString strSec="FileSetting";
	CString strEnt;
	for(int i =1;i<=12;i++)
	{
		strEnt.Format("Path%d",i);
		g_SetArray[i].m_strFileName=theApp.GetProfileString(strSec,strEnt,"");
		strEnt.Format("Collect%d",i);
		g_SetArray[i].m_DataCollection=theApp.GetProfileInt(strSec,strEnt,0);
		strEnt.Format("Ext%d",i);
		g_SetArray[i].m_Extension=theApp.GetProfileString(strSec,strEnt,"");
	}
	

	for (i = 0; i < 12; i++)
	{
		g_SetArray[i+1].m_strID.Format("%d",i+1);

		m_ListSet.InsertItem(i,g_SetArray[i+1].m_strID);
		if (g_SetArray[i+1].m_DataCollection)
		{
			m_ListSet.SetItemText(i,1,"o");
		}
		else
		{
			m_ListSet.SetItemText(i,1,"-");
		}
		m_ListSet.SetItemText(i,2,g_SetArray[i+1].m_strFileName);
		m_ListSet.SetItemText(i,3,g_SetArray[i+1].m_Extension);
	}
}

void CFileSettingDlg::UpdateListItem(int MYNo)
{
	if (g_SetArray[MYNo].m_DataCollection)
	{
		m_ListSet.SetItemText(MYNo-1,1,"O");
	}
	else
	{
		m_ListSet.SetItemText(MYNo-1,1,"-");
	}
	m_ListSet.SetItemText(MYNo-1,2,g_SetArray[MYNo].m_strFileName);
	m_ListSet.SetItemText(MYNo-1,3,g_SetArray[MYNo].m_Extension);
}

void CFileSettingDlg::OnButtonSet() 
{
	UpdateData(TRUE);
    if (m_strFileName.IsEmpty())
	{
		AfxMessageBox("Please input a file name.");
		return;
	}
	int i = m_ComboMyNo.GetCurSel();
	i++;
	g_SetArray[i].m_strFileName = m_strFileName;
	int n = atoi(m_strExt);
	m_strExt.Format("%.03d",n);
	g_SetArray[i].m_Extension = m_strExt;


	if (m_DataCollectionByNo == 0)
	{
		g_SetArray[i].m_DataCollection = TRUE;
	}
	else
	{
		g_SetArray[i].m_DataCollection = FALSE;
	}
	UpdateListItem(i);
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
	for(int i=1;i<=12;i++)
		UpdateListItem(i);
	
}

HBRUSH CFileSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	

	
	// TODO: Return a different brush if the default is not desired
	return hbr;
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

void CFileSettingDlg::OnDeltaposSpinExt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int n = (short)(pNMUpDown->iPos+pNMUpDown->iDelta);
	if (n<0) n=0;
	if (n>999) n = 999;
	m_strExt.Format("%.03d",n);
	UpdateData(FALSE);
	*pResult = 0;
}

void CFileSettingDlg::OnChangeEditExt() 
{
	UpdateData();
	m_spinExt.SetPos(atoi(m_strExt));
}



void CFileSettingDlg::OnSelchangeComboMyno() 
{
  	int i =m_ComboMyNo.GetCurSel();
	i++;

	m_strFileName = g_SetArray[i].m_strFileName;
	m_strExt = g_SetArray[i].m_Extension;
    m_DataCollectionByNo = (g_SetArray[i].m_DataCollection)?0:1;

	UpdateData(FALSE);

}
