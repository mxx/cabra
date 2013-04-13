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

CSpectrumDlg::CSpectrumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpectrumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpectrumDlg)
	//}}AFX_DATA_INIT
    pSpectrumWnd = NULL;
    pDC = NULL;
    for(int i=0;i<100;i++)
    {
        rawData.Speed[i]=100-i;
        rawData.State[i]=(i>50)?0x55:0xAA;
    }
    rawData.Longititude = 123.0*60.0 + 45.0 + 12.34/60.0;
    rawData.Latitude = 89.0*60.0 + 45.0 + 12.34/60.0;
    rawData.Altitude = 123456;
    rawData.strLicenseNumber = "XXXXXXXXXXXXXX";
    m_strVIC="XXXXXX";
    m_strVType="XXXX";
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
	
	pSpectrumWnd = GetDlgItem(IDC_GRAPH);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CSpectrumDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Do not call CDialog::OnPaint() for painting messages
	
	pSpectrumWnd->Invalidate();
	pSpectrumWnd->UpdateWindow();
	
	pDC = pSpectrumWnd->GetDC();
	CRect rect;
	pSpectrumWnd->GetClientRect(&rect);
    
	DrawGraph(pDC,0,0,rect.Width(),rect.Height());

	ReleaseDC(pDC);
	
}


void CSpectrumDlg::DrawGraph(CDC *pDC, int x, int y, int cx, int cy)
{
	pDC->SetBkMode(TRANSPARENT);
	CRect rect = DrawAxis(pDC,x,y,cx,cy);
	DrawData(pDC,rect.left,rect.top,rect.Width(),rect.Height());
    DrawLegend(pDC,rectSpeed.right+4,rectSpeed.top,(cx-rectSpeed.Width())/2,rectSpeed.Height());
}

int CSpectrumDlg::GetMaxCount(void)
{
    int i=0;
    int nMax = 0;
    for(i=0;i<(sizeof(rawData.Speed)/sizeof(rawData.Speed[0]));i++)
        nMax = max(rawData.Speed[i],nMax);
    if (nMax < 50) nMax = 50;
    return nMax;
    
}

int CSpectrumDlg::GetScaleMax(void)
{
   int nScaleMax = GetMaxCount();
   
   if (nScaleMax>50)
   {
       //make the Scale 100 times
       nScaleMax = ((nScaleMax/50) + ((nScaleMax%50)?1:0))*50;
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

CRect CSpectrumDlg::DrawAxis(CDC *pDC, int x, int y, int cx, int cy)
{
	int nScaleMax = GetScaleMax();
	int nScaleCount = 10;
	CString title;
	CRect rect(x,y,x+cx,y+cy);
	title.Format("%s","一二三四五六七八九十");
	pDC->FillSolidRect(&rect,RGB(250,250,150));
	CSize size = pDC->GetTextExtent(title);
	rect.DeflateRect(size.cx,size.cy+size.cy);
    rect.right = rect.left + (rect.Width()/4)*4;
    rect.top = rect.top + size.cy;
	pDC->FillSolidRect(&rect,RGB(255,255,255));

	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkColor(RGB(250,250,150));
	
    title.Format("车牌号码：%s，车牌分类：%s",m_strVIC,m_strVType);
    size = pDC->GetTextExtent(title);
    pDC->TextOut(x+(cx - size.cx)/2,y+size.cy/2,title);
    title.Format("驾驶证号码：%s",rawData.strLicenseNumber.c_str());
    pDC->TextOut(x+(cx - size.cx)/2,y+ size.cy + size.cy/2,title);
    
    
    
    int dx = size.cx; 
	int dy = (rect.Height()-(size.cy*10))/nScaleCount;

    rectState  = CRect(rect.left,rect.top,rect.right,rect.top + size.cy*10);
    rectSpeed  = CRect(rect.left,rect.top + size.cy*10,rect.right,rect.bottom);
	
    // x axis
    pDC->MoveTo(x+dx/2,rect.bottom);
    pDC->LineTo(rect.right+dx/2,rect.bottom);
    DrawScale(rectSpeed.left,rectSpeed.bottom,rectSpeed.Width(),5,20,true);
    DrawArrow(pDC,rect.right+dx/2-1,rect.bottom,15,3,false);
    dx = rect.Width() / 4;
    size = pDC->GetTextExtent("0");
    pDC->TextOut(rect.left - size.cx, rect.bottom , "0");
	for(int i=1;i<5;i++)
	{
		title.Format("%d",i*5);
		size = pDC->GetTextExtent(title);
		pDC->TextOut(rect.left + i * dx - size.cx/2, rect.bottom+12 , title);
       	CPen pen, *old_pen;
    	pen.CreatePen(PS_SOLID,1,RGB(220,220,220));
	    old_pen = pDC->SelectObject(&pen);
        pDC->MoveTo(rect.left + (i*dx),rect.bottom);
        pDC->LineTo(rect.left + (i*dx),rect.top);
        if (old_pen) pDC->SelectObject(old_pen);
        pen.DeleteObject();
	}
	pDC->TextOut(rect.left + 4 * dx + size.cx, rect.bottom + size.cy/2,"时间/s");
    //y axis
    pDC->MoveTo(rect.left,rect.bottom + size.cy);
    pDC->LineTo(rect.left,rect.top);
    DrawScale(rectSpeed.left,rectSpeed.top,rectSpeed.Height(),5,nScaleCount,false);
    DrawArrow(pDC,rectSpeed.left,rectSpeed.top-2*size.cy,3,-15,true);
	for(i =0;i < nScaleCount ;i++)
	{
		title.Format("%2d",(nScaleCount-i)*(nScaleMax/nScaleCount));
        size = pDC->GetTextExtent(title);
		pDC->TextOut(x+dx-size.cx-13, rectSpeed.top - size.cy/2 + dy * i, title );
 	}
    title = "速度/(km/h)";
    size = pDC->GetTextExtent(title);
    pDC->TextOut(rect.left - 5 - size.cx, rectSpeed.top-size.cy/2-size.cy,title);
    for (i=0; i<8;i++)
    {
        title.Format("%s(D%d)",config.strNameOf[i].empty()?"信号":config.strNameOf[i].c_str(),i);
        size = pDC->GetTextExtent(title);
		pDC->TextOut(rectState.left-3-size.cx, rectState.top+i*size.cy, title );
    }


	return rect;

}

void CSpectrumDlg::DrawArrow(CDC *pDC, int x, int y, int cx, int cy, bool bVer)
{
    pDC->MoveTo(x,y);
    if (bVer)
    {
        pDC->LineTo(x-cx,y-cy);
        pDC->MoveTo(x,y);
        pDC->LineTo(x+cx,y-cy);
    }
    else
    {
        pDC->LineTo(x-cx,y-cy);
        pDC->MoveTo(x,y);
        pDC->LineTo(x-cx,y+cy);
    }
}


void CSpectrumDlg::DrawPix(CDC *pDC, int nPix, int x, int y, double dx, double dy)
{
	CPen Pen;
	Pen.CreatePen(PS_SOLID,nPix,RGB(0,0,0));
	CPen* pen = pDC->SelectObject(&Pen);
    pDC->MoveTo(x+round(dx/2.0),y-round(rawData.Speed[0] * dy));
	for(int j=0;j<sizeof(rawData.Speed)/sizeof(rawData.Speed[0]);j++)
	{
    	int nx = x + round(j * dx + dx/2);
		int ny ;
		ny = y - round(rawData.Speed[j] * dy);
 		pDC->LineTo(nx,ny);
	}

	pDC->SelectObject(pen);
	Pen.DeleteObject();
}

void CSpectrumDlg::DrawStateData(double dx)
{
    CPen Pen;
    Pen.CreatePen(PS_SOLID,1,RGB(0,0,0));
    CPen* pen = pDC->SelectObject(&Pen);
    for(int i=0;i<8;i++)
    {
        int ny = rectState.top+rectState.Height()/20+((rectState.Height()/10)*i);
        pDC->MoveTo(rectState.left+round(dx/2.0), ny);
        for(int j=0;j<sizeof(rawData.State)/sizeof(rawData.State[0]);j++)
        {
            int v = rawData.State[j] & (1<<i);
            int nx = rectState.left + round(j * dx + dx/2);
            if (v) pDC->LineTo(nx,ny);
            else pDC->MoveTo(nx,ny);
        }
    }
	pDC->SelectObject(pen);
	Pen.DeleteObject();
}

void CSpectrumDlg::DrawData(CDC *pDC, int x,int y, int cx, int cy)
{
	
	int nPix = 1;
	if (pDC->IsPrinting()) 
		nPix = MulDiv(3, pDC->GetDeviceCaps(LOGPIXELSY), 96); 

	double dy = (rectSpeed.Height()*1.0)/GetScaleMax();
	double dx = (double)rectSpeed.Width() / 100.0;
	
	DrawPix(pDC,nPix,rectSpeed.left,rectSpeed.bottom,dx,dy);
    DrawStateData(dx);
	
}



void CSpectrumDlg::DrawLegend(CDC *pDC, int x,int y, int cx, int cy)
{
	if (pDC->IsPrinting()) return;
	int nx,ny;
	int spaceX,spaceY,starX;

	spaceX = pDC->GetTextExtent("M").cx/2;
	spaceY = pDC->GetTextExtent("M").cy;
	starX = pDC->GetTextExtent("*").cx/2;
    CSize size = pDC->GetTextExtent(_T("停车日期、时间:"));
	CRect rect(x,y,x+cx-4,y+size.cy*8);
	pDC->FillSolidRect(&rect,RGB(255,255,255));
	pDC->DrawEdge(&rect,EDGE_ETCHED,BF_RECT);
	pDC->SetBkColor(RGB(255,255,255));

	nx = rect.left+ 2*starX;
	ny = rect.top+ spaceY;
    pDC->TextOut(nx,ny,_T("停车日期、时间:"));
    CTime stopTime(rawData.tEnd);
    ny += spaceY;
    pDC->TextOut(nx,ny,stopTime.Format("%Y-%m-%d"));
    ny += spaceY;
    pDC->TextOut(nx,ny,stopTime.Format("%H:%M:%S"));
    ny += spaceY;
    CString str;
    double minu,sec ;
    sec = modf(rawData.Longititude,&minu);
        
    str.Format("经度:%.0f°%.0f\' %2.2f\"", floor(minu/60),fmod(minu,60.0),sec*60);
    pDC->TextOut(nx,ny,str);

    ny += spaceY;
    sec = modf(rawData.Latitude,&minu);
    str.Format("纬度:%.0f°%.0f\' %2.2f\"", floor(minu/60),fmod(minu,60.0),sec*60);
    pDC->TextOut(nx,ny,str);

    ny += spaceY;
    str.Format("高度:%d m",rawData.Altitude);
    pDC->TextOut(nx,ny,str);
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



void CSpectrumDlg::DrawScale(int startX, int startY, int nLongth, int minalD, int majorD, bool bHorizon)
{
    double deltaX = 0;
    double deltaY = 0;
    int zero ;
    if (bHorizon)
    {
        zero = startX;
        deltaX = (nLongth*1.0)/(minalD*majorD);
        for(int i=0;i<majorD;i++)
        {
            pDC->MoveTo(startX,startY);
            pDC->LineTo(startX,startY+10);
            for(int j=1;j<minalD;j++)
            {
                pDC->MoveTo(startX+round(deltaX*j),startY);
                pDC->LineTo(startX+round(deltaX*j),startY+5);
            }
            startX =zero + round(deltaX*minalD*(i+1));
        }
        pDC->MoveTo(startX,startY);
        pDC->LineTo(startX,startY+10);
    }
    else
    {
        zero = startY;
        deltaY = (nLongth*1.0)/(minalD*majorD);
        for(int i=0;i<majorD;i++)
        {
            pDC->MoveTo(startX,startY);
            pDC->LineTo(startX-10,startY);
            for(int j=1;j<minalD;j++)
            {
                pDC->MoveTo(startX,startY+round(deltaY*j));
                pDC->LineTo(startX-5,startY+round(deltaY*j));
            }
            startY =zero + round(deltaY*minalD*(i+1));;
        }
        pDC->MoveTo(startX,startY);
        pDC->LineTo(startX-10,startY);
    }
}


