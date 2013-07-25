// USBFileLoading.cpp : implementation file
//

#include "stdafx.h"
#include "analyzer.h"
#include "USBFileLoading.h"
#include "USBDataFilev2012.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUSBFileLoading dialog


CUSBFileLoading::CUSBFileLoading(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBFileLoading::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUSBFileLoading)
	m_strProgress = _T("");
	m_strMsg = _T("");
	//}}AFX_DATA_INIT
    m_bStop=FALSE;
    m_pSelf=NULL;
}


void CUSBFileLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBFileLoading)
	DDX_Text(pDX, IDC_STATIC_PROGESS, m_strProgress);
	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUSBFileLoading, CDialog)
	//{{AFX_MSG_MAP(CUSBFileLoading)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBFileLoading message handlers

void CUSBFileLoading::OnCancel() 
{
	m_bStop = TRUE;
	CDialog::OnCancel();
}


void CUSBFileLoading::loadData(CString &strFilePath)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    
    _splitpath( (LPCTSTR)strFilePath, drive, dir, fname, ext );
    CString strFileName;
    strFileName.Format("%s.%s",fname,ext);
    USBDataFilev2012 usbfile;
    if (!usbfile.ParseFileName((LPCTSTR)strFileName))
    {
        CString strP;
        strP.Format("文件名%s不合规范,取消读取？",(LPCTSTR)strFilePath);
        if (AfxMessageBox((LPCTSTR)strP,MB_YESNO,NULL)==IDYES)
            return;
    }
    
    try
    {
        string strDump;
        string strPlat;
        usbfile.ReadFromFile((LPCTSTR)strFilePath);
  
        strDump = VTDRRecord::UTF8ToGB2312(usbfile.m_strDecodeText);
        CString strTmp = strDump.c_str();
        strTmp.Replace("\n","\r\n");
        m_strMsg = strTmp;
        UpdateData(FALSE);

        for(int i =0 ;i < 21;i++)
        {
            USBDataFilev2012::DataSet& data = usbfile.GetDataList(i);
            if (&data)
            {
                list<VTDRRecord*>::iterator it = data.begin();
                string str;
                str = usbfile.DataBlockName[(*it)->GetDataCode()];
                CString strN;
                strN.Format("%d条记录\r\n",data.size());
                str += (LPCTSTR)strN;
                strDump += str;
                
                if (i == VTDRRecord::VehicleInfo)
                {
                    strPlat = VTDRRecord::GB2312ToUTF8(((VTDRVehicleInfo*)(*it))->strPlateNumber);
                }
                if (i==VTDRRecord::SpeedRecord)
                    m_ds.SaveSpeedRecord(strPlat.c_str(),*(VTDRSpeedRecord*)(*it));
            }
           // m_strMsg += strDump.c_str();
            //UpdateData(FALSE);
            
        }

    }
    catch(USBDataFileException& e)
    {
        AfxMessageBox(e.strType.c_str());
    }
    catch(DataStoreException& ex)
    {
        AfxMessageBox(ex.what());
    }
}

void CUSBFileLoading::DoLoad()
{
    list<CString>::iterator it;
    m_ds.Open();
    for(it = m_listFiles.begin(); it != m_listFiles.end();it++)
    {
        loadData(*it);
        Invalidate(TRUE);
    }

}

void CUSBFileLoading::OnOK() 
{
	CDialog::OnOK();
    m_strMsg="";
    m_listFiles.clear();
    UpdateData(FALSE);
}



