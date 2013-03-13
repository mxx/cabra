// SpectrumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Analyzer.h"
#include "Config.h"
#include "SpectrumDlg.h"


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
	//}}AFX_DATA_INIT
	pSpectrumWnd = NULL;

	nActiveIndex = 0;
	
	bLog = false;
}



void CSpectrumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectrumDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpectrumDlg, CDialog)
	//{{AFX_MSG_MAP(CSpectrumDlg)
	ON_WM_PAINT()
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
	int nScaleMax = 0;
	int nScaleCount =0;
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
		CString str ;
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


bool CSpectrumDlg::isOpenedAt(LPCTSTR szPath,int& n)
{
	list<RawData>::iterator it;
	int i=0;

	return false;
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
