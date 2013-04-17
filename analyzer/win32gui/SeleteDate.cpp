// SeleteDate.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "SeleteDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SeleteDate dialog


SeleteDate::SeleteDate(CWnd* pParent /*=NULL*/)
	: CDialog(SeleteDate::IDD, pParent)
{
	//{{AFX_DATA_INIT(SeleteDate)
	m_nRec = 0;
	m_timeEnd = 0;
	m_timeStart = 0;
	//}}AFX_DATA_INIT
}


void SeleteDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SeleteDate)
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_ctlStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_ctlEnd);
	DDX_Text(pDX, IDC_EDIT_REC, m_nRec);
	DDV_MinMaxInt(pDX, m_nRec, 0, 1000);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_ETIME, m_timeEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_STIME, m_timeStart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SeleteDate, CDialog)
	//{{AFX_MSG_MAP(SeleteDate)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SeleteDate message handlers

BOOL SeleteDate::OnInitDialog() 
{
	CDialog::OnInitDialog();
    
    if (!m_timeStart.GetTime())
    {
        CTime tmNow = CTime::GetCurrentTime();
        m_timeStart = tmNow;
        m_timeEnd = tmNow;
    }

    m_ctlStart.SetTime(&m_timeStart);
    m_ctlEnd.SetTime(&m_timeEnd);
    UpdateData(FALSE);
    m_ctlStart.SetFormat("yyyy-MM-dd"); 
    m_ctlEnd.SetFormat("yyyy-MM-dd"); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SeleteDate::OnCancelMode() 
{

}

void SeleteDate::OnOK() 
{
    CDialog::OnOK();
    CTime t;
    if (GDT_VALID == m_ctlStart.GetTime(t))
	{
		m_timeStart=CTime(t.GetYear(),t.GetMonth(),t.GetDay(),
        m_timeStart.GetHour(),m_timeStart.GetMinute(),m_timeStart.GetSecond());
	}
    m_ctlEnd.GetTime(t);
    m_timeEnd=CTime(t.GetYear(),t.GetMonth(),t.GetDay(),
        m_timeEnd.GetHour(),m_timeEnd.GetMinute(),m_timeEnd.GetSecond());

	
}
