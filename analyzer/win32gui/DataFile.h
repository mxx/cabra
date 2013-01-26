// DataFile.h: interface for the CDataFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAFILE_H__3AF7DAD3_0CA6_4931_A8EB_991ED79F3EE1__INCLUDED_)
#define AFX_DATAFILE_H__3AF7DAD3_0CA6_4931_A8EB_991ED79F3EE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Packet.h"


#pragma pack(1)
typedef struct {
	Group_Data group;
	char CR;
	char LF;
} Group_Line;

typedef struct {
	Titles titles;
	char CR;
	char LF;
} Title_Line;

typedef struct {
	CPMDPM_Field data;
	char CR;
	char LF;
} Data_Line;

typedef struct {
	char No[3];
	char sp0;
	Channel_Data data[10];
	char CR;
	char LF;
} Spectrum_Line;

#pragma pack()

class CDataFile  
{
public:
	bool GetGroup(Group_Line& group);
	void SetDPM(CString& strA,CString& strB);
	void SetEff(CString& strA, CString& strB);
	void SetGross(CString& strA, CString& strB);
	void GetDataLine(Data_Line& data);
	bool SaveAs(LPCTSTR szFileName);
	bool Load();
	bool Save(LPCTSTR szFile, CPacket& packet);
	void Close(void);
	int GetFieldValue(LPCTSTR szName,CString& strValue);
	int GetFieldIndex(LPCTSTR szName);
	bool IsValidFile(void);
	int Open(void);
	int Open(LPCTSTR szFileName);
	int GetSpectrumData(int* pArray,int len);
	
	CDataFile();
	virtual ~CDataFile();
	int nSpectrum[4000];
protected:
	void SaveSpectrumData(Spectrum_Line& szData);
	int chopFirstCSVField(CString& strLine,CString& strField);
	int readLine(CString& strLine);
	bool gotoSpetrumStart(void);
    CString strFileName;
	FILE* fData;
	Group_Line group;
	Title_Line title;
	Data_Line data;
	
};

#endif // !defined(AFX_DATAFILE_H__3AF7DAD3_0CA6_4931_A8EB_991ED79F3EE1__INCLUDED_)
