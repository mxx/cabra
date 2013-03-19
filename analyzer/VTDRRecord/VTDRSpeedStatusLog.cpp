/*
 * VTDRSpeedStatusLog.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRSpeedStatusLog.h"

const char* VTDRSpeedStatusLog::decodeType[]={
		"",
		"Normal",
		"Abnormal"
};

VTDRSpeedStatusLog::VTDRSpeedStatusLog() :
		tStart(0), tEnd(0), Status(Normal)
{
	cDataCode = SpeedStateLog;
}

VTDRSpeedStatusLog::~VTDRSpeedStatusLog()
{

}

int VTDRSpeedStatusLog::Read(const char* buf)
{
	SpeedStatusLog* ptrRec = (SpeedStatusLog*) buf;
	Status = (eSpeedStatus)ptrRec->SpeedStatus;
	tStart = ToSystime(ptrRec->StartTime);
	tEnd = ToSystime(ptrRec->EndTime);
	for (int i = 0; i < 60; i++)
	{
		recSpeed[i] = ptrRec->Speeds[i].recSpeed;
		refSpeed[i] = ptrRec->Speeds[i].refSpeed;
	}
	return sizeof(*ptrRec);
}

string& VTDRSpeedStatusLog::Write(string& buf)
{
	SpeedStatusLog log;
	log.SpeedStatus = Status;
	ToBCDTime(tStart, log.StartTime);
	ToBCDTime(tEnd, log.EndTime);
	for (int i = 0; i < 60; i++)
	{
		log.Speeds[i].recSpeed = recSpeed[i];
		log.Speeds[i].refSpeed = refSpeed[i];
	}
	buf.append((const char*) &log, sizeof(log));
	return buf;
}

string& VTDRSpeedStatusLog::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << endl;
	stream << "Type:" << decodeType[mapType(Status)] << endl;
	stream << "start time:" << ctime(&tStart);
	stream << "end time:" << ctime(&tEnd);
	stream <<  "seconds" << "\t" << "speed" << "\t" << "refSpeed" << endl;;
	for(int i=0;i<60;i++)
	{
		stream << i << "\t" << recSpeed[i] << "\t" << refSpeed[i] << endl;
	}
	stream << endl;
	return buf = stream.str();
}
