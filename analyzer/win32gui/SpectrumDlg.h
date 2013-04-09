#if !defined(AFX_SPECTRUMDLG_H__54AE311D_C503_43CD_8339_AC9875BE6351__INCLUDED_)
#define AFX_SPECTRUMDLG_H__54AE311D_C503_43CD_8339_AC9875BE6351__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpectrumDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpectrumDlg dialog
#include <list>
#include "VTDRDetailRecord.h"
using namespace std;

class CSpectrumDlg : public CDialog
{
// Construction
public:
	CSpectrumDlg(CWnd* pParent = NULL);   // standard constructor
    VTDRDetailRecord rawData;
// Dialog Data
	//{{AFX_DATA(CSpectrumDlg)
	enum { IDD = IDD_DIALOG_SPECTRUM };
	//}}AFX_DATA
	double m_nTime;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectrumDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawStateAxis(int x,int y,int cx,int cy);
    CWnd* pSpectrumWnd;
	void initColorList();
	void DrawPix(CDC *pDC,int nPix, int x,int y, double dx, double dy);
	void DrawPage(CDC& dc,int x,int y,int cx,int cy);
    int GetMaxCount(void);
    int GetScaleMax();
    int AxisYScaleCount(int& nMax, int nMin);
	void formatString(CString& str, int dx,int cy);
	
	//int nSpectrunData[5][4000];
	void DrawData(CDC *pDC,int x,int y, int cx,int cy);
	CRect DrawAxis(CDC *pDC, int x, int y, int cx, int cy);
    void DrawArrow(CDC *pDC, int x, int y, int cx, int cy,bool bVer);
	void DrawGraph(CDC* pDC, int x, int y, int cx, int cy);
	void DrawLegend(CDC* pDC, int x, int y, int cx, int cy);
	void DrawTableText(CDC& dc, int x, int y,int dx,int dy,int row,int column,CString str);
	void DrawTableTextRight(CDC& dc, int x, int y,int dx,int dy,int row,int column,CString str);
	void DrawTable(CDC& dc, int x, int y,int dx,int dy,int row,int column);
    CDC* pDC;
    CRect rectState;
    CRect rectSpeed;
	// Generated message map functions
	//{{AFX_MSG(CSpectrumDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    int yUnit;
	list<COLORREF> rgb;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRUMDLG_H__54AE311D_C503_43CD_8339_AC9875BE6351__INCLUDED_)
