// SpectrumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "Config.h"
#include "SpectrumDlg.h"
#include "SelectList.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int round(double x)
{
	double i = ceil(x);
	return ((i-x)<0.5)?(int)i:(int)(i-1);
}

/////////////////////////////////////////////////////////////////////////////
// CSpectrumDlg dialog
char CSpectrumDlg::szDataFilter[] = "Data File|*.???||";
char CSpectrumDlg::szAWSFilter[] = "AWS File(*.aws)|*.aws||";

CSpectrumDlg::CSpectrumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpectrumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpectrumDlg)
	m_strAGROSS = _T("");
	m_strBGROSS = _T("");
	m_strESCR = _T("");
	m_strSCCR = _T("");
	m_strADPM = _T("");
	m_strAEFF = _T("");
	m_strBDPM = _T("");
	m_strBEFF = _T("");
	m_strCurveName = _T("");
	m_strAchLL = _T("");
	m_strAchUL = _T("");
	m_strBchLL = _T("");
	m_strBchUL = _T("");
	//}}AFX_DATA_INIT
	pSpectrumWnd = NULL;
	setMF();
	nActiveIndex = 0;
	
	bLog = false;
}



void CSpectrumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectrumDlg)
	DDX_Control(pDX, IDC_BUTTON_LOG, m_btnLog);
	DDX_Control(pDX, IDC_SPIN_BU, m_spinBU);
	DDX_Control(pDX, IDC_SPIN_BL, m_spinBL);
	DDX_Control(pDX, IDC_SPIN_AU, m_spinAU);
	DDX_Control(pDX, IDC_SPIN_AL, m_spinAL);
	DDX_Text(pDX, IDC_EDIT_AGROSS, m_strAGROSS);
	DDV_MaxChars(pDX, m_strAGROSS, 20);
	DDX_Text(pDX, IDC_EDIT_BGROSS, m_strBGROSS);
	DDV_MaxChars(pDX, m_strBGROSS, 20);
	DDX_Text(pDX, IDC_EDIT_ESCR, m_strESCR);
	DDV_MaxChars(pDX, m_strESCR, 20);
	DDX_Text(pDX, IDC_EDIT_SCCR, m_strSCCR);
	DDV_MaxChars(pDX, m_strSCCR, 20);
	DDX_Text(pDX, IDC_EDIT_ADPM, m_strADPM);
	DDV_MaxChars(pDX, m_strADPM, 20);
	DDX_Text(pDX, IDC_EDIT_AEFF, m_strAEFF);
	DDV_MaxChars(pDX, m_strAEFF, 20);
	DDX_Text(pDX, IDC_EDIT_BDPM, m_strBDPM);
	DDV_MaxChars(pDX, m_strBDPM, 20);
	DDX_Text(pDX, IDC_EDIT_BEFF, m_strBEFF);
	DDV_MaxChars(pDX, m_strBEFF, 20);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strCurveName);
	DDX_Text(pDX, IDC_EDIT_ACHLL, m_strAchLL);
	DDX_Text(pDX, IDC_EDIT_ACHUL, m_strAchUL);
	DDX_Text(pDX, IDC_EDIT_BCHLL, m_strBchLL);
	DDX_Text(pDX, IDC_EDIT_BCHUL, m_strBchUL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpectrumDlg, CDialog)
	//{{AFX_MSG_MAP(CSpectrumDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILEDIA, OnButtonOpenAWD)
	ON_BN_CLICKED(IDC_BUTTON_FILE_OPEN, OnButtonFileOpen)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_LOG, OnButtonLog)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectrumDlg message handlers

BOOL CSpectrumDlg::PreTranslateMessage(MSG* pMsg) 
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

BOOL CSpectrumDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	pSpectrumWnd = GetDlgItem(IDC_SPECTRUM);
	m_spinAL.SetRange(1,4000);
	m_spinAU.SetRange(1,4000);
	m_spinBL.SetRange(1,4000);
	m_spinBU.SetRange(1,4000);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CSpectrumDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Do not call CDialog::OnPaint() for painting messages
	
	pSpectrumWnd->Invalidate();
	pSpectrumWnd->UpdateWindow();
	
	CDC *pDC = pSpectrumWnd->GetDC();
	CRect rect;
	pSpectrumWnd->GetClientRect(&rect);

	DrawGraph(pDC,0,0,rect.Width(),rect.Height());

	ReleaseDC(pDC);
	
}


void CSpectrumDlg::DrawGraph(CDC *pDC, int x, int y, int cx, int cy)
{
	pDC->SetBkMode(TRANSPARENT);
	CRect rect = DrawAxis(pDC,x,y,cx,cy);
	if (listData.size())
	{
		DrawData(pDC,rect.left,rect.top,rect.Width(),rect.Height());
	}
}

CRect CSpectrumDlg::DrawAxis(CDC *pDC, int x, int y, int cx, int cy)
{
	if (bLog) return DrawLogAxis(pDC,x,y,cx,cy);

	int nScaleMax = GetScaleMax();
	int nScaleCount = AxisYScaleCount(nScaleMax,0);
	CString title;
	CRect rect(x,y,x+cx,y+cy);
	title.Format("%d",nScaleMax*100);
	pDC->FillSolidRect(&rect,RGB(250,250,150));
	CSize size = pDC->GetTextExtent(title);
	rect.DeflateRect(size.cx,size.cy+size.cy/2);
	if(pDC->IsPrinting()) 
		pDC->FillSolidRect(&rect,RGB(255,255,255));
	else
		pDC->FillSolidRect(&rect,RGB(0,0,0));

	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkColor(RGB(250,250,150));
	int dx = size.cx/6; //space to border
	int dy = rect.Height()/nScaleCount;
	
	
	for(int i =0;i <= nScaleCount ;i++)
	{
		title.Format("%2d",(nScaleCount-i)*(nScaleMax/nScaleCount));
		pDC->TextOut(x+dx,rect.top - size.cy/2 + dy * i, title );
	}
    
	dx = rect.Width() / 4;
	for( i=0;i<5;i++)
	{
		title.Format("%d",i*1000);
		size = pDC->GetTextExtent(title);
		pDC->TextOut(rect.left + i * dx - size.cx/2, rect.bottom , title);
	}
	pDC->TextOut(rect.left + 4 * dx + size.cx/2, rect.bottom,"ch");

	return rect;

}

CRect CSpectrumDlg::DrawLogAxis(CDC *pDC, int x, int y, int cx, int cy)
{
	CString title;
	int nScaleMax = GetScaleMax();
	int nScaleCount;
	if (nScaleMax<100)
		nScaleCount = 5;
	else
		nScaleCount = AxisYScaleCountLog(log10(nScaleMax),(double)0);
	title.Format("%d",nScaleMax*100);
	CRect rect(x,y,x+cx,y+cy);
	pDC->FillSolidRect(&rect,RGB(250,250,150));
	CSize size = pDC->GetTextExtent(title);
	rect.DeflateRect(size.cx,size.cy+size.cy/2);
	if(pDC->IsPrinting())
		pDC->FillSolidRect(&rect,RGB(255,255,255));
	else
		pDC->FillSolidRect(&rect,RGB(0,0,0));

	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkColor(RGB(250,250,150));
	
	double nLogCount = log10(nScaleMax);
	
	int dx = size.cx/6; //space to border
	double dy = rect.Height()/nLogCount;
	int nScale;
	for(int i =0;i<nScaleCount;i++)
	{
		if (nScaleMax < 100)
		{
			nScale = (i==nScaleCount)?1:(nScaleCount-i)*(nScaleMax/nScaleCount);
		}
		else
		{
			nScale = (int)pow(10,(nScaleCount-1-i));
		}
		title.Format("%d",nScale);
		pDC->TextOut(x+dx,rect.bottom - dy * log10(nScale) - size.cy/2, title );
	}
    
	dx = rect.Width() / 4;
	for( i=0;i<5;i++)
	{
		title.Format("%d",i*1000);
		size = pDC->GetTextExtent(title);
		pDC->TextOut(rect.left + i * dx - size.cx/2, rect.bottom , title);
	}
	pDC->TextOut(rect.left + 4 * dx + size.cx/2, rect.bottom,"ch");

	return rect;

}

void CSpectrumDlg::DrawPix(CDC *pDC, int nPix, int x, int y, double dx, double dy, RawData &data)
{
	CPen Pen;
	Pen.CreatePen(PS_SOLID,nPix,data.rgb);
	CPen* pen = pDC->SelectObject(&Pen);
	for(int j=0;j<4000;j++)
	{
		int nx = x + round(j * dx);
		int ny ;
		if  (bLog)
			ny = y - round(log10(data.nSpetrum[j]) * dy);
		else
			ny = y - round(data.nSpetrum[j] * dy);
		pDC->MoveTo(nx,ny);
		pDC->LineTo(nx,ny);
	}
	pDC->SelectObject(pen);
	Pen.DeleteObject();
}

void CSpectrumDlg::DrawData(CDC *pDC, int x,int y, int cx, int cy)
{
	if (!listData.size()) return;
	CPen pen, *old_pen;
	pen.CreatePen(PS_SOLID,3,RGB(0,255,0));
	old_pen = pDC->SelectObject(&pen);
	list<RawData>::iterator it;
	int nPix = 3;
	if (pDC->IsPrinting()) 
		nPix = MulDiv(3, pDC->GetDeviceCaps(LOGPIXELSY), 96); 

	double dy;
	if (bLog)
		dy = cy / log10(GetScaleMax());
	else
		dy = (double)cy / GetScaleMax();
	double dx = (double)cx / 4000.0;
	int i=0;
	RawData& curData = GetListItem(nActiveIndex);
	for(it=listData.begin();it!=listData.end();it++)
	{
		if (nActiveIndex==i++)
		{
			if (pDC->IsPrinting()) break;
		    continue;
		}

		if (!pDC->IsPrinting())
			DrawPix(pDC,nPix,x,y+cy,dx,dy,*it);
	}
	
	DrawPix(pDC,nPix,x,y+cy,dx,dy,curData);
	
	if (old_pen) pDC->SelectObject(old_pen);
	DrawLegend(pDC,x,y,cx,cy);
}



void CSpectrumDlg::DrawLegend(CDC *pDC, int x,int y, int cx, int cy)
{
	if (pDC->IsPrinting()) return;
	int nx,ny;
	int spaceX,spaceY,starX;

	spaceX = pDC->GetTextExtent("M").cx;
	spaceY = pDC->GetTextExtent("M").cy;
	starX = pDC->GetTextExtent("*").cx;
	CRect rect(x+(cx*4)/5,y+cy/7,x+cx-cx/20,y+cy/7+7*spaceY);
	pDC->FillSolidRect(&rect,RGB(255,255,255));
	
	pDC->SetBkColor(RGB(255,255,255));

	nx = rect.left+ 2*starX;
	ny = rect.top+ spaceY;
	for(int i=0;i<listData.size();i++)
	{
		if(i>4) break;
		RawData& data = GetListItem(i);
		CString str = data.strName;
		int dx,dy;
		dx = pDC->GetTextExtent(str).cx;
		dy = pDC->GetTextExtent(str).cy;
		if (dx > rect.Width()-2*spaceX)
		{
			formatString(str,dx, rect.Width()-2*spaceX);
		}
		
		if (i==nActiveIndex) 
		{
			pDC->SetTextColor(RGB(0,0,0));
			pDC->TextOut(nx - starX -starX/2,ny+spaceX/8,"*");
		}
		pDC->SetTextColor(data.rgb);
		pDC->TextOut(nx,ny,str);
		ny += dy;
	}
}

void CSpectrumDlg::OnButtonOpenAWD() 
{
	CFileDialog dlg(TRUE,"awd",NULL,OFN_FILEMUSTEXIST,"AWS File(*.aws)|*.aws||",this->GetParent());
	if (dlg.DoModal()==IDOK)
	{
		m_strCurvePath = dlg.GetPathName();
		m_strCurveName = dlg.GetFileName();
		UpdateData(FALSE);
	}
}

void CSpectrumDlg::OnButtonFileOpen() 
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_ALLOWMULTISELECT ,szDataFilter,this->GetParent());
	TCHAR szFileName[5*(MAX_PATH+1)+1]={0};
	dlg.m_ofn.lpstrFile = szFileName;
	dlg.m_ofn.nMaxFile = 5*(MAX_PATH+1);
	if (dlg.DoModal() == IDOK)
	{
		if (listData.size())
			SaveActive(GetListItem(nActiveIndex));

		POSITION fileNamesPosition = dlg.GetStartPosition();
        int iCtr = 0;
		CString fileName[5];
        while(fileNamesPosition != NULL)
        {
              fileName[iCtr] = dlg.GetNextPathName(fileNamesPosition);
              iCtr++;
			  if (iCtr==5) break;
        }  

		int i=0;
		while(iCtr--)
		{
			if (listData.size() > 4)
			{
				MessageBox("Open too many files, olny 5 file can be displayed.");
				break;
			}
			int n;
			if (isOpenedAt(fileName[i],n))
			{
				readData(fileName[i],GetListItem(n));
				nActiveIndex = n;
				i++;
			}
			else
			{
				if (!LoadData(fileName[i++])) 
				{
					CString strMsg;
					strMsg.Format("Load file %s failed.",fileName[i-1]);
					MessageBox(strMsg);
				}
			}
		};

		if (i)
		{
			if(listData.size()) 
				setActiveData(GetListItem(nActiveIndex));

			Invalidate();   
			UpdateWindow();
			UpdateData(FALSE);
		}
	}
}



void CSpectrumDlg::OnButtonSelect() 
{
	CSelectList dlg(this);
	list<RawData>::iterator it;
	int i=0;
	for(it=listData.begin();it!=listData.end();it++)
	{
		dlg.m_strList[i++]=((*it).strName);
		if (i>4) break;
	}
	dlg.m_nSelIndex = nActiveIndex;

	if (listData.size())
		SaveActive(GetListItem(nActiveIndex));
	if (dlg.DoModal()==IDOK)
	{
		nActiveIndex = dlg.m_nSelIndex;
		if (listData.size())
		{
			setActiveData(GetListItem(nActiveIndex));
		}
		else
		{
			RawData data;
			setActiveData(data);
		}
		UpdateData(FALSE);
		Invalidate();
		UpdateWindow();
	}
}

void CSpectrumDlg::OnButtonDelete() 
{
	CSelectList dlg(this);
	dlg.m_strTitle = "Delete";
		list<RawData>::iterator it;
	int i=0;
	for(it=listData.begin();it!=listData.end();it++)
	{
		dlg.m_strList[i++]=((*it).strName);
		if (i>4) break;
	}
	dlg.m_nSelIndex = nActiveIndex;
	if (dlg.DoModal()==IDOK)
	{
		deleteData(dlg.m_nSelIndex);
		if (listData.size())
		{
			setActiveData(GetListItem(nActiveIndex));
		}
		else
		{
			RawData data;
			setActiveData(data);
		}
		UpdateData(FALSE);
		Invalidate();
		UpdateWindow();
		
	}
}

double CSpectrumDlg::Factor(double Y)
{
	double p,q;
	p = pow(10.0,MF_a);
	p *= MF_A;
	p *= pow(Y,3);

	q = pow(10.0,MF_b);
	q *= MF_B;
	q *= pow(Y,2);

	p += q;

	q = pow(10,MF_c);
	q *= MF_C;
	q *= Y;

	p += q;

	q = pow(10,MF_d);
	q *= MF_D;

	return p + q;

}

void CSpectrumDlg::setMF()
{
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH); 
	strPath.ReleaseBuffer();
	strPath=strPath.Left(strPath.ReverseFind( '\\')+1);
	Config conf(strPath+CONF_FILENAME);
	MF_A = conf.GetMF_A();
	MF_B = conf.GetMF_B();
	MF_C = conf.GetMF_C();
	MF_D = conf.GetMF_D();
	MF_a = conf.GetMF_a();
	MF_b = conf.GetMF_b();
	MF_c = conf.GetMF_c();
	MF_d = conf.GetMF_d();
}

bool CSpectrumDlg::LoadData(LPCTSTR szPath)
{

		return false;
}



RawData& CSpectrumDlg::GetListItem(int n)
{
	list<RawData>::iterator it;
	int i=0;
	for(it=listData.begin();it!=listData.end();it++)
	{
		if(i++==n) return (*it);
	}
	return listData.back();
}

void CSpectrumDlg::formatString(CString &str,int dx, int cx)
{
	int pDX;
	pDX = dx / str.GetLength();
	int n = cx / pDX;
	CString strF;
	strF = str;
	LPCTSTR pChar = strF;
	for(int i=0;i<n-5;i++)
	{
		pChar = CharNext(pChar);
		if ((pChar - (LPCTSTR)strF)> (n-5))
		{
			pChar = CharPrev(strF,pChar);
			break;
		}
		if ((pChar - (LPCTSTR)strF)== (n-5))
			break;
	}
	strF = strF.Left(pChar-strF);
	strF += "~";
	strF += str.Right(n - strF.GetLength());
	str = strF;
}

void CSpectrumDlg::deleteData(int n)
{
	list<RawData>::iterator it;
	int i=0;

	for(it=listData.begin();it!=listData.end();it++)
	{
		if(i++==n) 
		{
			rgb.push_front((*it).rgb);
			listData.erase(it);
			break;
		}
	}

	if(nActiveIndex >= n) nActiveIndex--;

	if (!(nActiveIndex < listData.size()))
		nActiveIndex = listData.size() - 1;
	if (nActiveIndex < 0 ) nActiveIndex = 0;
}

void CSpectrumDlg::setActiveData(RawData &data)
{
	m_strESCR = data.strESCR;
	m_strSCCR = data.strSCCR;
	m_strAEFF = data.strAEFF;
	m_strBEFF = data.strBEFF;
	m_strADPM = data.strADPM;
	m_strBDPM = data.strBDPM;
	m_strAGROSS =data.strAGROSS;
	m_strBGROSS=data.strBGROSS;
	m_strAchLL = data.strALL;
	m_strAchUL = data.strAUL;
	m_strBchLL = data.strBLL;
	m_strBchUL = data.strBUL;
	m_nTime = atof(data.strTime);
}

bool CSpectrumDlg::isOpenedAt(LPCTSTR szPath,int& n)
{
	list<RawData>::iterator it;
	int i=0;

	for(it=listData.begin();it!=listData.end();it++)
	{
		if ((*it).strPath == szPath)
		{
			n = i;
			return true;
		}
		i++;
	}
	return false;
}

DWORD CSpectrumDlg::ChanelSum(int nLL, int nLU)
{
	RawData& data = GetListItem(nActiveIndex);

	if (nLL < 0) nLL = 1;
	if (nLU > 3999) nLU = 3999;

	int sum = 0;

	for(int i = nLL-1;i<=nLU;i++)
	{
		sum+=data.nSpetrum[i];
	}

	return sum;
}

void CSpectrumDlg::OnButtonLog() 
{
	if (bLog)
	{
		bLog = false;
		m_btnLog.SetWindowText("&LOG");
	}
	else
	{
		bLog = true;
		m_btnLog.SetWindowText("&LIN");
	}
	Invalidate();
	UpdateWindow();
}

int CSpectrumDlg::GetMaxCount(void)
{
	list<RawData>::iterator it;
	int i=0;
	int nMax = 0;
	for(it=listData.begin();it!=listData.end();it++)
	{
		for(i=0;i<4000;i++)
			nMax = max((*it).nSpetrum[i],nMax);
	}

	if (nMax < 50) nMax = 50;
	return nMax;

}

int CSpectrumDlg::GetScaleMax()
{
   int nScaleMax = GetMaxCount();

   if (nScaleMax>50)
	{
		//make the Scale 100 times
		nScaleMax = ((nScaleMax/100) + ((nScaleMax%100)?1:0))*100;
	}
   return nScaleMax;
}

int CSpectrumDlg::AxisYScaleCount(int& nMax, int nMin)
{
	int count = nMax - nMin;
	
	double div = pow(10, floor(log10(count)-1));
	int nDivSum=(int)div;
	while (count>10)
	{
		count =(nMax-nMin)/nDivSum;
		if (count > 10) nDivSum+=(int)div;
	};
	if (count * div < 100)
	{
		div =10;
		count = (nMax - nMin)/div;
	}
	if (count==0)
	{
		count=1;
	}
	if ((nMax-nMin)%nDivSum) 
	{
		count+=1;
		nMax = count*nDivSum;
	}
	return count;
}

int CSpectrumDlg::AxisYScaleCountLog(double nMax, double nMin)
{
	int count = floor(nMax) - ceil(nMin)+1;
	
	return count;
}

void CSpectrumDlg::OnButtonPrint() 
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


void CSpectrumDlg::DrawPage(CDC &dc, int x, int y, int cx, int cy)
{

}

void CSpectrumDlg::GetCurrentFileName(CString &str)
{
	str.Empty();
	list<RawData>::iterator it;
	int i=0;
	for(it=listData.begin();it!=listData.end();it++)
	{
		if(i++==nActiveIndex) 
		{
			str = (*it).strPath;
			return;
		}
	}
}

void CSpectrumDlg::DrawTable(CDC& dc, int x, int y,int dx,int dy,int row,int column)
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

void CSpectrumDlg::DrawTableText(CDC& dc, int x, int y,int dx,int dy,int row,int column,CString str)
{
	CSize size = dc.GetTextExtent(str);
	dc.TextOut(x+(column-1)*dx+(dx-size.cx)/2,y+(row-1)*dy+(dy- size.cy)/2,str);
}

void CSpectrumDlg::DrawTableTextRight(CDC& dc, int x, int y,int dx,int dy,int row,int column,CString str)
{
	CSize size = dc.GetTextExtent(str);
	dc.TextOut(x+(column-1)*dx + (dx-size.cx-(dy-size.cy)), y+(row-1)*dy+(dy- size.cy)/2,str);
}

void CSpectrumDlg::SaveActive(RawData &data)
{
	UpdateData();
	data.strAEFF=	m_strAEFF ;
	data.strBEFF=	m_strBEFF ;
	data.strADPM=	m_strADPM ; 
	data.strBDPM=	m_strBDPM ;
	data.strAGROSS=	m_strAGROSS;
	data.strBGROSS=	m_strBGROSS;
	data.strALL=	m_strAchLL ;
	data.strAUL=	m_strAchUL ;
	data.strBLL=	m_strBchLL ;
	data.strBUL=	m_strBchUL ;
}



void CSpectrumDlg::initColorList()
{
	rgb.clear();
	rgb.push_back(RGB(200,100,100));
	rgb.push_back(RGB(150,100,250));
	rgb.push_back(RGB(100,150,150));
	rgb.push_back(RGB(75,250,75));
	rgb.push_back(RGB(250,75,75));
}

void CSpectrumDlg::readData(LPCTSTR szFile, RawData &rawData)
{

}
