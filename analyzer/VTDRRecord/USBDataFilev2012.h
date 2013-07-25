/*
 * USBDataFilev2012.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef USBDATAFILEV2012_H_
#define USBDATAFILEV2012_H_
#include <string>
#include <list>
#include <map>
#include "VTDRRecord.h"
#include "VTDRVersion.h"
#include "VTDRDriverInfo.h"
#include "VTDRRealTime.h"
#include "VTDROderMeter.h"
#include "VTDRPulseModulus.h"
#include "VTDRVehicleInfo.h"
#include "VTDRVehicleConfigure.h"
#include "VTDRUniqCode.h"
#include "VTDRSpeedRecord.h"
#include "VTDRPositionRecord.h"
#include "VTDRDetailRecord.h"
#include "VTDROvertimeDriveRecord.h"
#include "VTDRDriverIDRecord.h"
#include "VTDROutPoweredRecord.h"
#include "VTDRParameterModifyRecord.h"
#include "VTDRSpeedStatusLog.h"
#include <exception>

using namespace std;

class USBDataFileException: public exception
{
public:
	USBDataFileException(const char* szType) throw()
	{
		strType = szType;
	}
	;
	~USBDataFileException() throw()
	{
	}
	;
	string strType;
};

class USBDataFilev2012
{
public:
	USBDataFilev2012();
	virtual ~USBDataFilev2012();

	typedef list<VTDRRecord*> DataSet;

	bool ParseFileName(const char* szFileName);
	VTDRRecord* PushData(VTDRRecord* ptrRecord);
	void WriteToFile(const char* szFolder);
	void ReadFromFile(const char* szFileName);
	string& GenerateFileName();
	DataSet& GetDataList(int idx);
	static void initMap();
	static map<int, const char*> DataBlockName;
	string m_strDecodeText;
protected:
	string strPlateCode;
	time_t tRecordTime;
	string strFileName;

#ifdef _WIN32
#pragma pack(1)
#define PACK
#else
#define PACK  __attribute__ ((packed))
#endif

	typedef struct _USBDataBlock
	{
		unsigned char cDataCode;
		unsigned char cDataName[18];
		unsigned int nDataLength;
	} PACK USBDataBlock;

#ifdef _WIN32
#pragma pack()
#endif

	unsigned short nDataBlockNumber;

	map<int, DataSet> Datas;

	char checkSum(const string & str) const;
	void parseFile(const string& str);
	size_t readFileHead(const string& str);
	size_t readBlock(const string& str, int index);
	VTDRRecord* generateRecord(VTDRRecord::DataCode code);
};

#endif /* USBDATAFILEV2012_H_ */
