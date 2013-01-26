#if !defined(AFX_SPECTRUMDLG_H__54AE311D_C503_43CD_8339_AC9875BE6351__INCLUDED_)
#define AFX_SPECTRUMDLG_H__54AE311D_C503_43CD_8339_AC9875BE6351__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpectrumDlg.h : header file
//
typedef struct
{
	int nSpetrum[4000];
	CString strESCR;
	CString strSCCR;
	CString strAGROSS;
	CString strBGROSS;
	CString strTime;
	CString strName;
	CString strADPM;
	CString strBDPM;
	CString strAEFF;
	CString strBEFF;
	CString strALL;
	CString strAUL;
	CString strBLL;
	CString strBUL;
	CString strPath;
	COLORREF rgb;
} RawData;

/////////////////////////////////////////////////////////////////////////////
// CSpectrumDlg dialog
#include <list>
using namespace std;

class CSpectrumDlg : public CDialog
{
// Construction
public:
	CString m_strCurvePath;
	void GetCurrentFileName(CString& str);
	CSpectrumDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpectrumDlg)
	enum { IDD = IDD_DIALOG_SPECTRUM };
	CButton	m_btnLog;
	CSpinButtonCtrl	m_spinBU;
	CSpinButtonCtrl	m_spinBL;
	CSpinButtonCtrl	m_spinAU;
	CSpinButtonCtrl	m_spinAL;
	CString	m_strAGROSS;
	CString	m_strBGROSS;
	CString	m_strESCR;
	CString	m_strSCCR;
	CString	m_strADPM;
	CString	m_strAEFF;
	CString	m_strBDPM;
	CString	m_strBEFF;
	CString	m_strCurveName;
	CString	m_strAchLL;
	CString	m_strAchUL;
	CString	m_strBchLL;
	CString	m_strBchUL;
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
	void readData(LPCTSTR szFile, RawData& rawData);
	void initColorList();
	void DrawPix(CDC *pDC,int nPix, int x,int y, double dx, double dy, RawData& data);
	void SaveActive(RawData& data);
	void DrawPage(CDC& dc,int x,int y,int cx,int cy);
	int AxisYScaleCount(int& nMax,int nMin);
	int AxisYScaleCountLog(double nMax,double nMin);
	int GetScaleMax(void);
	int GetMaxCount(void);
	bool bLog;
	DWORD ChanelSum(int nLL,int nLU);
	bool isOpenedAt(LPCTSTR szPath,int& n);
	void setActiveData(RawData& data);
	void deleteData(int n);
	void formatString(CString& str, int dx,int cy);
	RawData& GetListItem(int n);
	bool LoadData(LPCTSTR szPath);
	void setMF(void);
	double Factor(double Y);
	
	list<RawData> listData;
	
	//int nSpectrunData[5][4000];
	void DrawData(CDC *pDC,int x,int y, int cx,int cy);
	CRect DrawAxis(CDC *pDC, int x, int y, int cx, int cy);
	CRect DrawLogAxis(CDC *pDC, int x, int y, int cx, int cy);
	void DrawGraph(CDC* pDC, int x, int y, int cx, int cy);
	void DrawLegend(CDC* pDC, int x, int y, int cx, int cy);
	void DrawTableText(CDC& dc, int x, int y,int dx,int dy,int row,int column,CString str);
	void DrawTableTextRight(CDC& dc, int x, int y,int dx,int dy,int row,int column,CString str);
	void DrawTable(CDC& dc, int x, int y,int dx,int dy,int row,int column);
	CWnd * pSpectrumWnd;

	// Generated message map functions
	//{{AFX_MSG(CSpectrumDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAwsFactor();
	afx_msg void OnPaint();
	afx_msg void OnButtonOpenAWD();
	afx_msg void OnButtonFileOpen();
	afx_msg void OnButtonFileSave();
	afx_msg void OnButtonSelect();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonAws();
	afx_msg void OnButtonLog();
	afx_msg void OnButtonPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static char szDataFilter[];
	static char szAWSFilter[];
	int nActiveIndex;
	
	list<COLORREF> rgb;
	//Model Factor
	double MF_A,MF_B,MF_C,MF_D,MF_a,MF_b,MF_c,MF_d;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRUMDLG_H__54AE311D_C503_43CD_8339_AC9875BE6351__INCLUDED_)
