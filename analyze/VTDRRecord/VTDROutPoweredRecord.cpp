/*
 * VTDROutPoweredRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDROutPoweredRecord.h"

VTDROutPoweredRecord::VTDROutPoweredRecord() :
		tTime(0), cType(0)
{
	cDataCode = OutPowered;
}

VTDROutPoweredRecord::~VTDROutPoweredRecord()
{
	// TODO Auto-generated destructor stub
}

int VTDROutPoweredRecord::Read(const char* buf)
{
	PowerLog* ptrLog = (PowerLog*) buf;
	tTime = ToSystime(ptrLog->vTime);
	cType = ptrLog->cType;
	return sizeof(*ptrLog);
}

string& VTDROutPoweredRecord::Write(string& buf)
{
	PowerLog log;
	ToBCDTime(tTime, log.vTime);
	log.cType = cType;
	buf.append((const char*) &log, sizeof(log));
	return buf;
}
