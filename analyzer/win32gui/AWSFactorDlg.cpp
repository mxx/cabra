// AWSFactorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "AWSFactorDlg.h"
#include "Config.h"
#include <math.h>
#include <float.h>
#include <limits>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAWSFactorDlg dialog
double round(double x)
{
	double i = ceil(x);
	return ((i-x)<0.5)?i:i-1;
}

CAWSFactorDlg::CAWSFactorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAWSFactorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAWSFactorDlg)
	m_nAchUL = 0;
	m_nAchLL = 0;
	m_nBchLL = 0;
	m_nBchUL = 0;
	m_dBA_a = "";
	m_dBA_b = "";
	m_dBA_d = "";
	m_dBA_c = "";
	m_dBch_a = "";
	m_dBch_b = "";
	m_dBch_c = "";
	m_dBch_d = "";
	m_dAch_a = "";
	m_dAch_b = "";
	m_dAch_c = "";
	m_dAch_d = "";
	m_strCurveName = _T("");
	m_ADPM = 0;
	m_BDPM = 0;
	//}}AFX_DATA_INIT
}


void CAWSFactorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CAWSFactorDlg)
	DDX_Control(pDX, IDC_STATIC_ORG, m_stcOrg);
	DDX_Control(pDX, IDC_BUTTON_FILE_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_FILE_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BUTTON_SET, m_btnSet);
	DDX_Text(pDX, IDC_EDIT_AUL, m_nAchUL);
	DDX_Text(pDX, IDC_EDIT_ALL, m_nAchLL);
	DDX_Text(pDX, IDC_EDIT_BLL, m_nBchLL);
	DDX_Text(pDX, IDC_EDIT_BUL, m_nBchUL);
	DDX_Text(pDX, IDC_EDIT_CURVENAME, m_strCurveName);
	DDX_Text(pDX, IDC_EDIT_ADPM, m_ADPM);
	DDV_MinMaxUInt(pDX, m_ADPM, 0, 9999999);
	DDX_Text(pDX, IDC_EDIT_BDPM, m_BDPM);
	DDV_MinMaxUInt(pDX, m_BDPM, 0, 9999999);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_EDIT_BAA, m_dBA_a);
		DDX_Text(pDX, IDC_EDIT_BAB, m_dBA_b);
		DDX_Text(pDX, IDC_EDIT_BAD, m_dBA_d);
		DDX_Text(pDX, IDC_EDIT_BAC, m_dBA_c);
		DDX_Text(pDX, IDC_EDIT_BCHA, m_dBch_a);
		DDX_Text(pDX, IDC_EDIT_BCHB, m_dBch_b);
		DDX_Text(pDX, IDC_EDIT_BCHC, m_dBch_c);
		DDX_Text(pDX, IDC_EDIT_BCHD, m_dBch_d);
		DDX_Text(pDX, IDC_EDIT_ACHA, m_dAch_a);
		DDX_Text(pDX, IDC_EDIT_ACHB, m_dAch_b);
		DDX_Text(pDX, IDC_EDIT_ACHC, m_dAch_c);
		DDX_Text(pDX, IDC_EDIT_ACHD, m_dAch_d);
	}
	
}


BEGIN_MESSAGE_MAP(CAWSFactorDlg, CDialog)
	//{{AFX_MSG_MAP(CAWSFactorDlg)
	ON_BN_CLICKED(IDC_BUTTON_SPECTRUM, OnButtonSpectrum)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_FILE_SAVE, OnButtonFileSave)
	ON_BN_CLICKED(IDC_BUTTON_FILE_OPEN, OnButtonFileOpen)
	ON_BN_CLICKED(IDC_BUTTON_PRNT, OnButtonPrint)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAWSFactorDlg message handlers

BOOL CAWSFactorDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
			break;
		case VK_ESCAPE:
			return TRUE;
			break;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAWSFactorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
    ULONG style;

	CRect rect;
	CRect rect0,rectL;
	m_stcOrg.GetWindowRect(&rect);
	ScreenToClient(&rect);
	
	for(int j=0;j<10;j++)
	{
	  rect.OffsetRect(0,rect.Height()+2);
	  rect0 = rect;
	  CString strLabel;
	  strLabel.Format("SAMPLE-%d",j+1);
	  rectL = rect;
	  rectL.OffsetRect(-1*rect.Width()-4,0);
	  lbl[j].Create(strLabel,WS_CHILD|WS_VISIBLE|SS_RIGHT,rectL,this);
	  lbl[j].SetFont(m_stcOrg.GetFont());
	  style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT;
	  for(int i=0;i<5;i++)
	  {
		  editSample[j][i].Create(style,
		  rect0,this,IDC_EDIT_SAMPLE+j*10+i);
	      editSample[j][i].SetFont(m_stcOrg.GetFont());
		  
		  rect0.OffsetRect(rect.Width()+1,0);
		  switch(i)
		  {
		  case 1:
			  rect0.OffsetRect(rect.Width()/5,0);
			  break;
		  }

	  };
	}
  
	style |= ES_READONLY;
	rect.OffsetRect(5*rect.Width()+4+rect.Width()/2,0);
	rect.OffsetRect(0,-10*(rect.Height()+2));
	for(j=0;j<10;j++)
	{
		rect.OffsetRect(0,rect.Height()+2);
		rect0 = rect;
		for(int i=5;i<8;i++)
		{
			editSample[j][i].Create(style,
				rect0,this,IDC_EDIT_SAMPLE+j*10+i);
			editSample[j][i].SetFont(m_stcOrg.GetFont());
			
			rect0.OffsetRect(rect.Width()+1,0);
		};
	}

	EnableItems(FALSE);

	RegisterHotKey(this->m_hWnd, 0x2000, MOD_CONTROL | MOD_ALT, 'C');

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAWSFactorDlg::OnButtonSpectrum() 
{
	::PostMessage(this->GetParent()->m_hWnd,WM_OPEN_DLG,OPEN_SPECTRUM,0);
	UpdateData();
}

LONG CAWSFactorDlg::OnHotKey(WPARAM wParam,LPARAM lParam)
{
    if (!IsWindowEnabled()) return 0;
	GetParent()->SetWindowText("AWS Factor Setting");
	m_btnSet.ShowWindow(TRUE);
	m_btnSave.ShowWindow(TRUE);
	EnableItems(TRUE);
    return 0;     
}

void CAWSFactorDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	UnregisterHotKey(this->m_hWnd,0x2000);   
	
}

void CAWSFactorDlg::OnButtonSet() 
{
	if (editSample[0][0].GetStyle()&ES_READONLY)
		return;

		CAWSFile awsFile;
		AWS_Setting set;
		AWS_CalCo co;
		GetSetting(set);
		if (awsFile.CalculateCoefficient(set,co))
			LoadCalCo(co);
		m_btnSet.ShowWindow(FALSE);
		
		EnableItems(FALSE);

}

void CAWSFactorDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if (bShow)
	{

		if(!m_strCurvePath.IsEmpty())
			OpenFile();
	}
	else
	{
		m_btnSet.ShowWindow(bShow);
		m_btnSave.ShowWindow(bShow);
		EnableItems(FALSE);
	}
}



void CAWSFactorDlg::OnButtonFileSave() 
{
	if ((editSample[0][0].GetStyle()&ES_READONLY) && 
		 !m_btnSave.IsWindowVisible())
		return;
	

	CFileDialog dlg(FALSE,"awd",NULL,0,"AWS File(*.aws)|*.aws||",this->GetParent());
	if (dlg.DoModal()==IDOK)
	{
		CAWSFile awsFile;
		AWS_Setting set;
		GetSetting(set);
		awsFile.WriteFile(dlg.GetPathName(),set);
		m_strCurveName = dlg.GetFileName();
		m_strCurvePath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CAWSFactorDlg::GetSetting(AWS_Setting &set)
{
	UpdateData();    
	set.nAch_LL = m_nAchLL;
	set.nAch_UL = m_nAchUL;
	set.nBch_LL = m_nBchLL;
	set.nBch_UL = m_nBchUL;
	set.nA_DPM = m_ADPM;
	set.nB_DPM = m_BDPM;
	double dbNaN = std::numeric_limits<double>::quiet_NaN();


	CString strVal;
	for(int i=0;i<10;i++)
	{
		editSample[i][0].GetWindowText(strVal);
		set.sample.dA_ratio[i] = strVal.IsEmpty()?dbNaN:atof((LPCTSTR)strVal);
		editSample[i][1].GetWindowText(strVal);
		set.sample.dA_CPM[i] = strVal.IsEmpty()?dbNaN:atof((LPCTSTR)strVal);
		editSample[i][2].GetWindowText(strVal);
		set.sample.dB_ratio[i] = strVal.IsEmpty()?dbNaN:atof((LPCTSTR)strVal);
		editSample[i][3].GetWindowText(strVal);
		set.sample.dB_A_CPM[i] = strVal.IsEmpty()?dbNaN:atof((LPCTSTR)strVal);
		editSample[i][4].GetWindowText(strVal);
		set.sample.dB_CPM[i] = strVal.IsEmpty()?dbNaN:atof((LPCTSTR)strVal);
	}
}

void CAWSFactorDlg::OnButtonFileOpen() 
{
	CFileDialog dlg(TRUE,"awd",NULL,OFN_FILEMUSTEXIST,"AWS File(*.aws)|*.aws||",this->GetParent());
	if (dlg.DoModal()==IDOK)
	{
		m_strCurvePath = dlg.GetPathName();
		m_strCurveName = dlg.GetFileName();
		OpenFile();
	}
}

void CAWSFactorDlg::LoadData(AWS_Setting &set)
{
	
	m_nAchLL = set.nAch_LL ;
	m_nAchUL = 	set.nAch_UL;
	m_nBchLL = set.nBch_LL;
	m_nBchUL = set.nBch_UL;
	m_ADPM = set.nA_DPM ;
	m_BDPM = set.nB_DPM; 
	
	CString strVal;
	for(int i=0;i<10;i++)
	{
		(_isnan(set.sample.dA_ratio[i]))?strVal.Empty():strVal.Format("%G",set.sample.dA_ratio[i]);
		editSample[i][0].SetWindowText(strVal);
		(_isnan(set.sample.dA_CPM[i]))?strVal.Empty():strVal.Format("%G",set.sample.dA_CPM[i]);
		editSample[i][1].SetWindowText(strVal);
		(_isnan(set.sample.dB_ratio[i]))?strVal.Empty():strVal.Format("%G",set.sample.dB_ratio[i]);
		editSample[i][2].SetWindowText(strVal);
		(_isnan(set.sample.dB_A_CPM[i]))?strVal.Empty():strVal.Format("%G",set.sample.dB_A_CPM[i] );
		editSample[i][3].SetWindowText(strVal);
		(_isnan(set.sample.dB_CPM[i] ))?strVal.Empty():strVal.Format("%G",set.sample.dB_CPM[i] );
		editSample[i][4].SetWindowText(strVal);
	}

	UpdateData(FALSE);
}

void CAWSFactorDlg::LoadData(AWS_Setting &set,AWS_CalCo& co)
{
	LoadData(set);
	LoadCalCo(co);
}

void CAWSFactorDlg::LoadCalCo(AWS_CalCo &co)
{
	CString strVal;
	for(int i=0;i<10;i++)
	{
		_isnan(co.cal.dA_Eff[i])?strVal.Empty():strVal.Format("%10G",co.cal.dA_Eff[i]);
		editSample[i][5].SetWindowText(strVal);
		_isnan(co.cal.dB_Eff[i])?strVal.Empty():strVal.Format("%10G",co.cal.dB_Eff[i]);
		editSample[i][6].SetWindowText(strVal);
		_isnan(co.cal.dBA_CPM[i])?strVal.Empty():strVal.Format("%10G",co.cal.dBA_CPM[i]);
		editSample[i][7].SetWindowText(strVal);
	}

	m_dAch_a.Format("%#4.4e",co.dAch_co[3]);
	m_dAch_b.Format("%#4.4e",co.dAch_co[2]);
	m_dAch_c.Format("%#4.4e",co.dAch_co[1]);
	m_dAch_d.Format("%#4.4e",co.dAch_co[0]);
	m_dBch_a.Format("%#4.4e",co.dBch_co[3]);
	m_dBch_b.Format("%#4.4e",co.dBch_co[2]);
	m_dBch_c.Format("%#4.4e",co.dBch_co[1]);
	m_dBch_d.Format("%#4.4e",co.dBch_co[0]);
	m_dBA_a.Format("%#4.4e",co.d_BA_co[3]);
	m_dBA_b.Format("%#4.4e",co.d_BA_co[2]);
	m_dBA_c.Format("%#4.4e",co.d_BA_co[1]);
	m_dBA_d.Format("%#4.4e",co.d_BA_co[0]);
	UpdateData(FALSE);
}

void CAWSFactorDlg::OpenFile(void)
{
		CAWSFile awsFile;
		AWS_Setting set;
		AWS_CalCo co;
		
		awsFile.ReadData(m_strCurvePath,set);
		if (awsFile.CalculateCoefficient(set,co))
			LoadData(set,co);
		else
		{
			MessageBox("Calculate coefficients fail! only sample data loaded.");
			LoadData(set);
		}
}

void CAWSFactorDlg::EnableItems(BOOL bEnable)
{
	for(int j=0;j<10;j++)
		for(int i=0;i<5;i++)
			editSample[j][i].SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_ALL))->SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_BLL))->SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_AUL))->SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_BUL))->SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_ADPM))->SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_BDPM))->SetReadOnly(!bEnable);
}

void CAWSFactorDlg::OnButtonPrint() 
{
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if (printDlg.DoModal() != IDOK)
		return;

	CWaitCursor wait;
	
	dc.Attach(printDlg.GetPrinterDC());
	dc.m_bPrinting=TRUE;
	
	DOCINFO di; 
	memset(&di,0,sizeof (DOCINFO));
	di.cbSize=sizeof (DOCINFO);
	
	BOOL bPrintingOK=dc.StartDoc(&di); //开始一个打印任务
	CPrintInfo Info;
	
	int cx,cy;
	int mcx,mcy;//margin
	cx = dc.GetDeviceCaps(HORZRES);
	mcx = cx/15;
	cy = dc.GetDeviceCaps(VERTRES);
	mcy = cy/20;
	Info.m_rectDraw.SetRect(mcx,mcy,cx-mcx,cy-mcy);

	Info.SetMinPage(1);
	Info.SetMaxPage(1);
		CFont font,*old_pf;
	int lfH = MulDiv(12, dc.GetDeviceCaps(LOGPIXELSY), 72); 
	font.CreateFont(
		lfH,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Times New Roman");                 // lpszFacename
	old_pf = dc.SelectObject(&font);


	dc.StartPage(); //开始一个新的打印页
	Info.m_nCurPage=1;
	
	DrawPage(dc,mcx,mcy,Info.m_rectDraw.Width(),Info.m_rectDraw.Height());
		
	bPrintingOK=(dc.EndPage() > 0); //打印页结束
	
	if (bPrintingOK)
		dc.EndDoc(); //一个打印任务结束
	else
		dc.AbortDoc(); //终止打印任务
    dc.SelectObject(old_pf);	
	dc.Detach(); //释放打印机DC
	
}

void CAWSFactorDlg::DrawPage(CDC &dc, int x, int y, int cx, int cy)
{
	int nCurrentY = y;

	dc.TextOut(x,nCurrentY,"<AWS Factor>");
	CSize size = dc.GetTextExtent("<AWS Factor>");
	CString strText;
	strText.Format("Curve Name: %s\n",m_strCurveName);
	int dy=size.cy/3;
	int dx=size.cy/3;

	nCurrentY+=size.cy+dy;
	
	dc.TextOut(x,nCurrentY,strText);

	nCurrentY+=size.cy+dy;

	int ndx,ndy;
	ndx = cx/6;
	ndy = size.cy+dy;
	DrawTable(dc,x,nCurrentY,ndx,ndy,3,3);
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,2,"LL");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,3,"UL");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,2,1,"A-ch");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,3,1,"B-ch");
	strText.Format("%d",m_nAchLL);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,2,strText);
	strText.Format("%d",m_nAchUL);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,3,strText);
	strText.Format("%d",m_nBchLL);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,3,2,strText);
	strText.Format("%d",m_nBchUL);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,3,3,strText);

	int nCurX = x + cx/2 + cx/8;
	DrawTable(dc,nCurX,nCurrentY,ndx,ndy,2,2);
	DrawTableText(dc,nCurX,nCurrentY,ndx,ndy,1,1,"A-DPM");
	DrawTableText(dc,nCurX,nCurrentY,ndx,ndy,1,2,"B-DPM");
	strText.Format("%u",m_ADPM);
	DrawTableTextRight(dc,nCurX,nCurrentY,ndx,ndy,2,1,strText);
	strText.Format("%u",m_BDPM);
	DrawTableTextRight(dc,nCurX,nCurrentY,ndx,ndy,2,2,strText);

	nCurrentY+= 4*ndy;

	size = dc.GetTextExtent("SAMPLE-10 ");
	ndx = (size.cx>ndx)?size.cx:ndx;
	DrawTable(dc,x,nCurrentY,ndx,ndy,11,3);
	for(int i=1;i<11;i++)
	{
		strText.Format("SAMPLE-%d",i);
		DrawTableText(dc,x,nCurrentY,ndx,ndy,i+1,1,strText);
	}
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,2,"A-RATIO");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,3,"A-CPM");

	for(i=1;i<11;i++)
	{
		editSample[i-1][0].GetWindowText(strText);
		DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,i+1,2,strText.Right(9));
		editSample[i-1][1].GetWindowText(strText);
		DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,i+1,3,strText.Right(9));
	}

	nCurX = x + 3*ndx + 15;
	ndx -=5;
	DrawTable(dc,nCurX,nCurrentY,ndx,ndy,11,3);
	DrawTableText(dc,nCurX,nCurrentY,ndx,ndy,1,1,"B-RATIO");
	DrawTableText(dc,nCurX,nCurrentY,ndx,ndy,1,2,"A-CPM");
	DrawTableText(dc,nCurX,nCurrentY,ndx,ndy,1,3,"B-CPM");
	for(i=1;i<11;i++)
	{
		editSample[i-1][2].GetWindowText(strText);
		DrawTableTextRight(dc,nCurX,nCurrentY,ndx,ndy,i+1,1,strText.Right(9));
		editSample[i-1][3].GetWindowText(strText);
		DrawTableTextRight(dc,nCurX,nCurrentY,ndx,ndy,i+1,2,strText.Right(9));
		editSample[i-1][4].GetWindowText(strText);
		DrawTableTextRight(dc,nCurX,nCurrentY,ndx,ndy,i+1,3,strText.Right(9));
	}

	nCurrentY+=12*ndy;
	ndx = cx/5;
	DrawTable(dc,x,nCurrentY,ndx,ndy,4,5);
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,2,"a");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,3,"b");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,4,"c");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,5,"d");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,2,1,"A-ch");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,3,1,"B-ch");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,4,1,"B/A ");


	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,2,m_dAch_a);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,3,m_dAch_b);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,4,m_dAch_c);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,5,m_dAch_d);

	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,3,2,m_dBch_a);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,3,3,m_dBch_b);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,3,4,m_dBch_c);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,3,5,m_dBch_d);
	

	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,4,2,m_dBA_a);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,4,3,m_dBA_b);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,4,4,m_dBA_c);
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,4,5,m_dBA_d);

	nCurrentY+=5*ndy;
	ndx = cx/6;
	DrawTable(dc,x,nCurrentY,ndx,ndy,11,4);
	for(i=1;i<11;i++)
	{
		strText.Format("SAMPLE-%d",i);
		DrawTableText(dc,x,nCurrentY,ndx,ndy,i+1,1,strText);
	}
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,2,"A-Eff");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,3,"B-Eff");
	DrawTableText(dc,x,nCurrentY,ndx,ndy,1,4,"B/A-CPM");
	for(i=1;i<11;i++)
	{
		editSample[i-1][5].GetWindowText(strText);
		DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,i+1,2,strText.Right(9));
		editSample[i-1][6].GetWindowText(strText);
		DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,i+1,3,strText.Right(9));
		editSample[i-1][7].GetWindowText(strText);
		DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,i+1,4,strText.Right(9));
	}

	nCurrentY+=12*ndy;
	ndx = cx/8;
	dc.TextOut(x,nCurrentY,"Model Factor");
	nCurrentY+=ndy;

	DrawTable(dc,x,nCurrentY,ndx,ndy,2,8);
	for(i=0;i<4;i++)
	{
		char buf[2]={0};
		buf[0]='A';
		buf[0]+=(char)i;
		DrawTableText(dc,x,nCurrentY,ndx,ndy,1,i+1,buf);
		buf[0]='a';
		buf[0]+=(char)i;
		DrawTableText(dc,x,nCurrentY,ndx,ndy,1,i+5,buf);
	}
	
	Config conf("config.ini");
	strText.Format("%10G",conf.GetMF_A());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,1,strText);	
	strText.Format("%10G",conf.GetMF_B());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,2,strText);	
	strText.Format("%10G",conf.GetMF_C());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,3,strText);	
	strText.Format("%10G",conf.GetMF_D());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,4,strText);	
	strText.Format("%10G",conf.GetMF_a());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,5,strText);	
	strText.Format("%10G",conf.GetMF_b());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,6,strText);	
	strText.Format("%10G",conf.GetMF_c());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,7,strText);	
	strText.Format("%10G",conf.GetMF_d());
	DrawTableTextRight(dc,x,nCurrentY,ndx,ndy,2,8,strText);	
}

void CAWSFactorDlg::DrawTable(CDC& dc, int x, int y,int dx,int dy,int row,int column)
{
	int ndx,ndy;
	int cx,cy;
	if (!row || !column) return;
	ndx = dx;
	ndy = dy;
	cx = dx*column;
	cy = dy*row;
	for(int i =0; i<=row;i++)
	{
		dc.MoveTo(x,y+i*ndy);
		dc.LineTo(x+cx,y+i*ndy);
	}

	for(i=0;i<=column;i++)
	{
		dc.MoveTo(x+i*ndx,y);
		dc.LineTo(x+i*ndx,y+cy);
	}
}

void CAWSFactorDlg::DrawTableText(CDC& dc, int x, int y,int dx,int dy,int row,int column,LPCTSTR str)
{
	CSize size = dc.GetTextExtent(str);
	dc.TextOut(x+(column-1)*dx+(dx-size.cx)/2,y+(row-1)*dy+(dy- size.cy)/2,str);
}

void CAWSFactorDlg::DrawTableTextRight(CDC& dc, int x, int y,int dx,int dy,int row,int column,LPCTSTR str)
{
	CSize size = dc.GetTextExtent(str);
	dc.TextOut(x+(column-1)*dx + (dx-size.cx-(dy-size.cy)), y+(row-1)*dy+(dy- size.cy)/2,str);
}

