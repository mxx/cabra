// DataFile.cpp: implementation of the CDataFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Analyzer.h"
#include "DataFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataFile::CDataFile()
{
	fData = NULL;
	strFileName.Empty();
}

CDataFile::~CDataFile()
{
    if  (fData) fclose(fData);
}


int CDataFile::GetSpectrumData(int *pArray, int len)
{
	if (fData==NULL) return 0;
	CString strLine;
	int n=0;
	
	try
	{
		if (IsValidFile())
		{
			gotoSpetrumStart();
			while(!feof(fData))
			{
				readLine(strLine);
				pArray[n++] = atoi(LPCTSTR(strLine));
				if (n>3999) break;
			};
		}
		else
		{
			return 0;
		}
	}
	catch(...)
	{
		return n;
	}
	return n;
}

int CDataFile::Open(LPCTSTR szFileName)
{
	if (fData != NULL)
	{
		fclose(fData);
		fData = NULL;
	}
	strFileName = szFileName;
	return Open();
}

int CDataFile::Open()
{
	fData = fopen((LPCTSTR)strFileName,"rb");
	if (fData == NULL)
	{
		perror((LPCTSTR)strFileName);
		return 0;
	}
	return 1;
}

bool CDataFile::IsValidFile()
{
	if (fData)
	{
		try
		{
			fseek(fData,0,SEEK_SET);
			char buf[1024];
			if (fgets(buf,1024,fData) &&
				fgets(buf,1024,fData))
			{
				if (strstr(buf,"MY NO")
					&& strstr(buf,"TIME")
					&& strstr(buf,"SCCR"))
				{
					if (fgets(buf,1024,fData)
						&& fgets(buf,1024,fData)
						&& !feof(fData))
						return true;
				}
			}
		}
		catch(...)
		{
			return false;
		}
	}
	return false;
}

bool CDataFile::gotoSpetrumStart()
{
	if (fData)
	{
		fseek(fData,0,SEEK_SET);
		CString strLine;
		if (readLine(strLine)
			&&readLine(strLine)&&readLine(strLine))
			return true;
	}
	return false;
}

int CDataFile::readLine(CString &strLine)
{
	if (fData)
	{
		char buf[1024]={0};
		if (fgets(buf,1024,fData))
		{
			strLine = buf;
			strLine.TrimRight();
			return strlen(buf);
		}
	}
	return 0;
}

int CDataFile::chopFirstCSVField(CString& strLine,CString& strField)
{
	if (strLine.GetLength())
	{
		strField.Empty();
		int n=strLine.Find(',');
		if ( n < 0) 
		{
			strField = strLine;
			strLine.Empty();
		}

		if (n >= 0)
		{
			strField = strLine.Left(n);
			strLine = strLine.Right(strLine.GetLength()-n-1);
		}

		return n;
	}

	return 0;
}

int CDataFile::GetFieldIndex(LPCTSTR szName)
{
	if (fData)
	{
		int n =0;
		fseek(fData,0,SEEK_SET);
		CString strLine;
		if (readLine(strLine)
			&&readLine(strLine))
		{
			CString strField;
			while(strLine.GetLength())
			{
				n++;
				chopFirstCSVField(strLine,strField);
				if (strField.Find(szName)>-1)
				{
					return n;
				}
			};
		}
	}
	return 0;
}

int CDataFile::GetFieldValue(LPCTSTR szName, CString &strValue)
{
	//strValue.Empty();
	int n = GetFieldIndex(szName);
	if (n)
	{
		fseek(fData,0,SEEK_SET);
		CString strLine;
		if (readLine(strLine)
			&&readLine(strLine)
			&&readLine(strLine))
		{
			CString strField;
			while(n && strLine.GetLength())
			{
				chopFirstCSVField(strLine,strField);
				n--;
			}
			if (n) 
			{
				return 0;
			}

			strField.TrimLeft();
			strField.TrimLeft('\"');
			strField.TrimLeft();
			strField.TrimRight('\"');
			strField.TrimRight();
			strValue = strField;
			return 1;
		}
	}
	return 0;
}

void CDataFile::Close()
{
	if (fData)
	{
		fclose(fData);
		fData = NULL;
	}
}



bool CDataFile::Save(LPCTSTR szFile, CPacket &packet)
{
	Close();
	fData = fopen(szFile,"ab");
	if (fData == NULL) return false;
	CString strGroup = packet.GetGroupData();
	CString strTitle = packet.GetTitleData();
	CString strData = packet.GetDMPData();
	CString strSpectrum = packet.GetSpetrumData();
	
	do
	{
		if(!strGroup.IsEmpty())
		{
			fprintf(fData,"%s\r\n",(LPCTSTR)strGroup);
			break;
		}
		if(!strTitle.IsEmpty())
		{
			fprintf(fData,"%s\r\n",(LPCTSTR)strTitle);
			break;
		}
		if(!strData.IsEmpty())
		{
			fprintf(fData,"%s\r\n",(LPCTSTR)strData);
			break;
		}
		if(!strSpectrum.IsEmpty())
		{
			SaveSpectrumData(*(Spectrum_Line*)((LPCTSTR)strSpectrum));
			break;
		}
		break;
	} while(1);

	Close();
	return true;
}

bool CDataFile::Load()
{
	if (strFileName.IsEmpty()) return false;
	if (!Open()) return false;
	if (!IsValidFile()) return false;
	CString strLine;
	rewind(fData);
	if (readLine(strLine)!=sizeof(group)) return false;
	memcpy(&group,(LPCTSTR)strLine,sizeof(group.group));
	group.CR = '\r';
	group.LF = '\n';
	if (readLine(strLine)!=sizeof(title)) return false;
	memcpy(&title,(LPCTSTR)strLine,sizeof(title));
	title.CR = '\r';
	title.LF = '\n';
	if (readLine(strLine)!=sizeof(data)) return false;
	memcpy(&data,(LPCTSTR)strLine,sizeof(data));
	data.CR = '\r';
	data.LF ='\n';
	return 4000==GetSpectrumData(nSpectrum,4000);
}

bool CDataFile::SaveAs(LPCTSTR szFileName)
{
	FILE* file = fopen(szFileName,"wb");
	if (!file) return false;
	
	if (fwrite(&group,1,sizeof(group),file)&&
	fwrite(&title,1,sizeof(title),file)&&
	fwrite(&data,1,sizeof(data),file))
	{
		for(int i=0;i<4000;i++)
		{
			if (fprintf(file,"%d\r\n",nSpectrum[i])<0)
			{
				fclose(file);return false;
			}
		}
		fclose(file);
		return true;
	}
	fclose(file);
	return false;
}

void CDataFile::GetDataLine(Data_Line &data)
{
	data = this->data;
}

void CDataFile::SetEff(CString& strA, CString& strB)
{
	double i;
	i = atof(strA);
	CString str;
	str.Format("% 9.2f",i);
	memcpy(data.data.A_EFF,str,sizeof(data.data.A_EFF));
	i = atof(strB);
	str.Format("% 9.2f",i);
	memcpy(data.data.B_EFF,str,sizeof(data.data.B_EFF));
	
}

void CDataFile::SetDPM(CString &strA, CString &strB)
{
	double i;
	i = atof(strA);
	CString str;
	str.Format("% 9.2f",i);
	memcpy(data.data.A_DPM,str,sizeof(data.data.A_DPM));
	i = atof(strB);
	str.Format("% 9.2f",i);
	memcpy(data.data.B_DPM,str,sizeof(data.data.B_DPM));
}

void CDataFile::SetGross(CString &strA, CString &strB)
{
	int i;
	i = atoi(strA);
	CString str;
	str.Format("% 7d",i);
	memcpy(data.data.A_GROSS,str,sizeof(data.data.A_GROSS));
	i = atoi(strB);
	str.Format("% 7d",i);
	memcpy(data.data.B_GROSS,str,sizeof(data.data.B_GROSS));
}

bool CDataFile::GetGroup(Group_Line &group)
{
	group=this->group;
	return true;
}

void CDataFile::SaveSpectrumData(Spectrum_Line& Data)
{
	if (fData)
	{
		CString data;
		for(int i=0;i<10;i++)
		{
			data=CString(Data.data[i].count,6);
			fprintf(fData,"%d\r\n",atoi((LPCTSTR)data));
		}
	}
}
