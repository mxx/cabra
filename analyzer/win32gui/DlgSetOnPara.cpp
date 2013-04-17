// DlgSetOnPara.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "DlgSetOnPara.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetOnPara dialog


CDlgSetOnPara::CDlgSetOnPara(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetOnPara::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetOnPara)
	m_nPara = 0;
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSetOnPara::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetOnPara)
	DDX_Text(pDX, IDC_EDIT_PARA, m_nPara);
	DDV_MinMaxUInt(pDX, m_nPara, 0, 4294967295);
	DDX_Text(pDX, IDC_STATIC_PARANAME, m_strTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetOnPara, CDialog)
	//{{AFX_MSG_MAP(CDlgSetOnPara)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetOnPara message handlers
