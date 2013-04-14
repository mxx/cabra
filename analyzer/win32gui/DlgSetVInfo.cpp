// DlgSetVInfo.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "DlgSetVInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetVInfo dialog


CDlgSetVInfo::CDlgSetVInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetVInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetVInfo)
	m_strPClass = _T("");
	m_strPNo = _T("");
	m_strVID = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSetVInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetVInfo)
	DDX_Text(pDX, IDC_EDIT_PCLASS, m_strPClass);
	DDV_MaxChars(pDX, m_strPClass, 12);
	DDX_Text(pDX, IDC_EDIT_PNO, m_strPNo);
	DDV_MaxChars(pDX, m_strPNo, 12);
	DDX_Text(pDX, IDC_EDIT_VID, m_strVID);
	DDV_MaxChars(pDX, m_strVID, 17);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetVInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgSetVInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetVInfo message handlers
