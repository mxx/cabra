#if !defined(AFX_TABLEDATA_H__BE2610FA_9A50_47E5_A500_6117E8247AEC__INCLUDED_)
#define AFX_TABLEDATA_H__BE2610FA_9A50_47E5_A500_6117E8247AEC__INCLUDED_
#include <list>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableData.h : header file
//
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CTableData window

class CTableData : public CListCtrl
{
// Construction
public:
	CTableData();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableData)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	list<CString> m_Heads;
	int InitTable(void);
	virtual ~CTableData();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTableData)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEDATA_H__BE2610FA_9A50_47E5_A500_6117E8247AEC__INCLUDED_)
