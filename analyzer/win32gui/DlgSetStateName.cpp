// DlgSetStateName.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "DlgSetStateName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetStateName dialog


CDlgSetStateName::CDlgSetStateName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetStateName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetStateName)
	m_strName1 = _T("");
	m_strName2 = _T("");
	m_strName3 = _T("");
	m_strName4 = _T("");
	m_strName5 = _T("");
	m_strName6 = _T("");
	m_strName7 = _T("");
	m_strName8 = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSetStateName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetStateName)
	DDX_Text(pDX, IDC_EDIT1, m_strName1);
	DDV_MaxChars(pDX, m_strName1, 10);
	DDX_Text(pDX, IDC_EDIT2, m_strName2);
	DDV_MaxChars(pDX, m_strName2, 10);
	DDX_Text(pDX, IDC_EDIT3, m_strName3);
	DDV_MaxChars(pDX, m_strName3, 10);
	DDX_Text(pDX, IDC_EDIT4, m_strName4);
	DDV_MaxChars(pDX, m_strName4, 10);
	DDX_Text(pDX, IDC_EDIT5, m_strName5);
	DDV_MaxChars(pDX, m_strName5, 10);
	DDX_Text(pDX, IDC_EDIT6, m_strName6);
	DDV_MaxChars(pDX, m_strName6, 10);
	DDX_Text(pDX, IDC_EDIT7, m_strName7);
	DDV_MaxChars(pDX, m_strName7, 10);
	DDX_Text(pDX, IDC_EDIT8, m_strName8);
	DDV_MaxChars(pDX, m_strName8, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetStateName, CDialog)
	//{{AFX_MSG_MAP(CDlgSetStateName)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetStateName message handlers
