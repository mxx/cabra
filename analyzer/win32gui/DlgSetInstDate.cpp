// DlgSetInstDate.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "DlgSetInstDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetInstDate dialog


CDlgSetInstDate::CDlgSetInstDate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetInstDate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetInstDate)
	m_tInstDate = 0;
	m_tInstTime = 0;
	//}}AFX_DATA_INIT
}


void CDlgSetInstDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetInstDate)
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_INSTDATE, m_tInstDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_INSTTIME, m_tInstTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetInstDate, CDialog)
	//{{AFX_MSG_MAP(CDlgSetInstDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetInstDate message handlers

void CDlgSetInstDate::OnOK() 
{
	
    m_tInstDate = CTime(m_tInstDate.GetYear(),m_tInstDate.GetMonth(),m_tInstDate.GetDay(),
                m_tInstTime.GetHour(),m_tInstTime.GetMinute(),m_tInstTime.GetSecond());
	CDialog::OnOK();
}
