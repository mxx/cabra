// TableData.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "TableData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableData

CTableData::CTableData()
{
}

CTableData::~CTableData()
{
}


BEGIN_MESSAGE_MAP(CTableData, CListCtrl)
	//{{AFX_MSG_MAP(CTableData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableData message handlers



int CTableData::InitTable()
{
    list<CString>::iterator it;
    int i = 0;
    for(it = m_Heads.begin(); it != m_Heads.end(); it++)
    {
        InsertColumn(0,LPCTSTR(*it)); 
        i++;
    }

    return i;
}
