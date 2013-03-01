/*
 * USBDataFilev2012.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */
#include "USBDataFilev2012.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iconv.h>
#include "TraceLog.h"

map<int, const char*> USBDataFilev2012::DataBlockName;

USBDataFilev2012::USBDataFilev2012() :
		tRecordTime(0), nDataBlockNumber(0)
{
	strPlateCode = "未知车牌";
	initMap();
}

USBDataFilev2012::~USBDataFilev2012()
{
	if (!Datas.empty())
	{
		map<int, DataSet>::iterator it;
		for (it = Datas.begin(); it != Datas.end(); it++)
		{
			DataSet::iterator dit;
			for (dit = it->second.begin(); dit != it->second.end(); dit++)
			{
				delete (*dit);
			}
			it->second.clear();
		}
	}
}

VTDRRecord* USBDataFilev2012::PushData(VTDRRecord* ptrRecord)
{
	VTDRRecord::DataCode code = (VTDRRecord::DataCode) ptrRecord->GetDataCode();
	if (Datas.count(code))
	{
		if (code < VTDRRecord::SpeedRecord)
		{
			delete Datas[code].front();
			Datas[code].clear();
			Datas[code].push_back(ptrRecord);
		}
		else
			Datas[ptrRecord->GetDataCode()].push_back(ptrRecord);
	}
	else
	{
		nDataBlockNumber++;
		TRACE("block count %d",nDataBlockNumber);
		DataSet records;
		records.push_back(ptrRecord);
		Datas[ptrRecord->GetDataCode()] = records;
	}
	return ptrRecord;
}

void USBDataFilev2012::WriteToFile(const char* szFolder)
{
	string buf;
	unsigned short blocks = htons(nDataBlockNumber);
	buf.append((const char*) &blocks, sizeof(blocks));
	map<int, DataSet>::iterator it;
	for (it = Datas.begin(); it != Datas.end(); it++)
	{
		if (it->second.empty())
			continue;
		DataSet::iterator data_set;
		USBDataBlock block =
		{ 0 };
		block.cDataCode = it->second.front()->GetDataCode();
		string strName = DataBlockName[block.cDataCode];
		VTDRRecord::utf8togb2312(strName.c_str(), strName.size(),
				(char*) block.cDataName, sizeof(block.cDataName));

		string strBlock;
		for (data_set = it->second.begin(); data_set != it->second.end();
				data_set++)
		{
			(*data_set)->Write(strBlock);
		}
		block.nDataLength = htonl(strBlock.size());
		buf.append((const char*) &block, sizeof(block));
		buf.append(strBlock);
	}
	buf.append(1, checkSum(buf));

	GenerateFileName();
	FILE* fp = fopen(strFileName.c_str(), "wb");
	if (fp)
	{
		int n = fwrite((void*) buf.data(), 1, buf.size(), fp);
		fclose(fp);
		if (n != 1)
		{
			throw USBDataFileException("write fail");
		}
		return;
	}
	throw USBDataFileException("open file for write fail");
}

string& USBDataFilev2012::GenerateFileName()
{
	struct tm tmTime;
	tRecordTime = (tRecordTime) ? tRecordTime : time(NULL);
	localtime_r(&tRecordTime, &tmTime);
	char buf[64] =
	{ 0 };
	sprintf(buf, "D%02u%02u%02u_%02u%02u_%s.VDR", tmTime.tm_year + 1900 - 2000,
			tmTime.tm_mon + 1, tmTime.tm_mday, tmTime.tm_hour, tmTime.tm_min,
			strPlateCode.c_str());
	strFileName = buf;
	return strFileName;
}

void USBDataFilev2012::initMap()
{
	if (DataBlockName.empty())
	{
		DataBlockName[0] = "执行标准版本年号";
		DataBlockName[1] = "当前驾驶人信息";
		DataBlockName[2] = "实时时间";
		DataBlockName[3] = "累计行驶里程";
		DataBlockName[4] = "脉冲系数";
		DataBlockName[5] = "车辆信息";
		DataBlockName[6] = "状态信号配置信息";
		DataBlockName[7] = "记录仪唯一性编号";
		DataBlockName[8] = "行驶速度记录";
		DataBlockName[9] = "位置信息记录";
		DataBlockName[0x10] = "事故疑点记录";
		DataBlockName[0x11] = "超时驾驶记录";
		DataBlockName[0x12] = "驾驶人身份记录";
		DataBlockName[0x13] = "外部供电记录";
		DataBlockName[0x14] = "参数修改记录";
		DataBlockName[0x15] = "速度状态日志";
	}
}

bool USBDataFilev2012::ParseFileName(const char* szFileName)
{
	struct stFileName
	{
		char prefix;
		char year[2];
		char month[2];
		char day[2];
		char sp1;
		char hour[2];
		char minute[2];
		char sp2;
		char plateNumber[8];
		char sp3;
		char ext[3];
	} FileName, *pFileName;
	tRecordTime = 0;
	if (!szFileName
			|| VTDRRecord::utf8togb2312(szFileName, strlen(szFileName),
					(char*) &FileName, sizeof(FileName)))
		return false;

	pFileName = &FileName;

	struct tm tmTime =
	{ 0 };
	string strTmp;

	if (pFileName->prefix == 'D' && pFileName->sp1 == '_'
			&& pFileName->sp2 == '_' && pFileName->sp3 == '.')
	{

		strTmp.assign((const char*) pFileName->year, 2);
		tmTime.tm_year = atoi(strTmp.c_str()) + 2000 - 1900;
		strTmp.assign((const char*) pFileName->month, 2);
		tmTime.tm_mon = atoi(strTmp.c_str()) - 1;
		strTmp.assign((const char*) pFileName->day, 2);
		tmTime.tm_mday = atoi(strTmp.c_str());
		strTmp.assign(pFileName->hour, 2);
		tmTime.tm_hour = atoi(strTmp.c_str());
		strTmp.assign(pFileName->minute, 2);
		tmTime.tm_min = atoi(strTmp.c_str());
		tRecordTime = mktime(&tmTime);

		char plate[32];
		VTDRRecord::gb2312toutf8(pFileName->plateNumber,
				sizeof(pFileName->plateNumber), plate, sizeof(plate));
		strPlateCode = plate;
		return true;
	}

	return false;
}

void USBDataFilev2012::ReadFromFile(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "rb");
	string strBuf;
	if (fp)
	{
		char buf[10240];
		int n = 0;
		do
		{
			n = fread(buf, 1, sizeof(buf), fp);
			TRACE("read %d bytes",n);
			if (n)
				strBuf.append(buf, n);
		} while (n == sizeof(buf));
		fclose(fp);
	}
	else
	{
		strBuf = "open file";
		strBuf += szFileName;
		strBuf += "fail";
		throw USBDataFileException(strBuf.c_str());
	}

	if (strBuf.size())
		parseFile(strBuf);
	else
		TRACE("no data read");
}

char USBDataFilev2012::checkSum(const string& str) const
{
	char sum = 0x00;
	TRACE("");
	for (string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		sum ^= (*it);
	}
	return sum;
}

void USBDataFilev2012::parseFile(const string& str)
{
	if (checkSum(str))
		throw USBDataFileException("checksum error");
	TRACE("Checksum OK");
	size_t index = readFileHead(str);
	TRACE("Total %d Block",nDataBlockNumber);
	int nFileBlock = nDataBlockNumber;
	nDataBlockNumber = 0;
	while (index < (str.size() - 1))
	{
		index += readBlock(str, index);
	}; TRACE("total read %d blocks",nDataBlockNumber);
	if (nDataBlockNumber != nFileBlock)
	{
		ERROR("Declared Block number:%d, actual read %d blocks",nFileBlock,nDataBlockNumber);
		throw USBDataFileException("unmatched data blocks");
	}
}

size_t USBDataFilev2012::readFileHead(const string& str)
{
	nDataBlockNumber = str[0];
	nDataBlockNumber = (nDataBlockNumber << 8) + str[1];
	return 2;
}

size_t USBDataFilev2012::readBlock(const string& str, int index)
{
	USBDataBlock* ptrBlock = (USBDataBlock*) (str.data() + index);
	size_t nLength = (size_t) ntohl(ptrBlock->nDataLength);
	TRACE("block length %d, code %d",nLength,ptrBlock->cDataCode);
	if (nLength + index > str.size())
	{
		ERROR("bad block size");
		throw USBDataFileException("bad block size");
	}
	VTDRRecord* ptrRecord = NULL;
	char szBlockName[64] =
	{ 0 };
	VTDRRecord::gb2312toutf8((const char*) ptrBlock->cDataName,
			sizeof(ptrBlock->cDataName), szBlockName, 64);
	TRACE("blockName:%s",szBlockName);
	size_t n = 0;
	while (nLength > n)
	{
		ptrRecord = generateRecord((VTDRRecord::DataCode) ptrBlock->cDataCode);

		if (!ptrRecord)
		{
			throw USBDataFileException("generate record fail");
		}

		n += ptrRecord->Read(str.data() + sizeof(USBDataBlock) + index + n);
		TRACE("Read:%d[Def:%d]",n,nLength);

		if (nLength < n)
		{
			ERROR("bad data size");
			//throw USBDataFileException("bad data size");
		}

		string buf;
		ptrRecord->Dump(buf);
		TRACE("\n%s",buf.c_str());
		PushData(ptrRecord);
	};

	return nLength + sizeof(USBDataBlock);
}

VTDRRecord* USBDataFilev2012::generateRecord(VTDRRecord::DataCode code)
{
	VTDRRecord* ptrRecord = NULL;
	switch (code)
	{
	case VTDRRecord::Version:
		TRACE("Version");
		ptrRecord = new VTDRVersion();
		break;
	case VTDRRecord::CurrentDriver:
		TRACE("CurrentDriver");
		ptrRecord = new VTDRDriverInfo();
		break;
	case VTDRRecord::RealTime:
		TRACE("RealTime");
		ptrRecord = new VTDRRealTime();
		break;
	case VTDRRecord::OderMeter:
		TRACE("OderMeter");
		ptrRecord = new VTDROderMeter();
		break;
	case VTDRRecord::PulseModulu:
		ptrRecord = new VTDRPulseModulus();
		break;
	case VTDRRecord::VehicleInfo:
		ptrRecord = new VTDRVehicleInfo();
		break;
	case VTDRRecord::StateConfig:
		ptrRecord = new VTDRVehicleConfigure();
		break;
	case VTDRRecord::UniqCode:
		ptrRecord = new VTDRUniqCode();
		break;
	case VTDRRecord::SpeedRecord:
		TRACE("SpeedRecord");
		ptrRecord = new VTDRSpeedRecord();
		break;
	case VTDRRecord::PositionRecord:
		ptrRecord = new VTDRPositionRecord();
		break;
	case VTDRRecord::AccidentSuspectPoint:
		ptrRecord = new VTDRDetailRecord();
		break;
	case VTDRRecord::OverTimeDriving:
		ptrRecord = new VTDROvertimeDriveRecord();
		break;
	case VTDRRecord::DriverInfo:
		ptrRecord = new VTDRDriverIDRecord();
		break;
	case VTDRRecord::OutPowered:
		ptrRecord = new VTDROutPoweredRecord();
		break;
	case VTDRRecord::ParameterModify:
		ptrRecord = new VTDRParameterModifyRecord();
		break;
	case VTDRRecord::SpeedStateLog:
		ptrRecord = new VTDRSpeedStatusLog();
		break;
	default:
		break;
	}
	return ptrRecord;
}
