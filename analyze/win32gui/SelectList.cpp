// SelectList.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "SelectList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectList dialog


CSelectList::CSelectList(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strTitle = "Select";
}


void CSelectList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectList)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectList, CDialog)
	//{{AFX_MSG_MAP(CSelectList)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectList message handlers



void CSelectList::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(bShow)
	{
		SetWindowText(m_strTitle);
		m_list.ResetContent();
		for(int i=0;i<5;i++)
		{
			if (m_strList[i].IsEmpty()) break;
			m_list.InsertString(i,m_strList[i]);
		}
		m_list.SetCurSel(m_nSelIndex);
	}
}

void CSelectList::OnOK() 
{
	m_nSelIndex = m_list.GetCurSel();
	CDialog::OnOK();
}
