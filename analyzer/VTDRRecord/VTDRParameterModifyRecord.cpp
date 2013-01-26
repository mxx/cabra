/*
 * VTDRParameterModifyRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRParameterModifyRecord.h"

VTDRParameterModifyRecord::VTDRParameterModifyRecord() :
		tTime(0), cType(0)
{
	cDataCode = ParameterModify;
}

VTDRParameterModifyRecord::~VTDRParameterModifyRecord()
{

}

int VTDRParameterModifyRecord::Read(const char* buf)
{
	ModifyLog* ptrLog = (ModifyLog*) buf;
	tTime = ToSystime(ptrLog->vTime);
	cType = ptrLog->cType;
	return sizeof(*ptrLog);
}

string& VTDRParameterModifyRecord::Write(string& buf)
{
	ModifyLog log =
	{ 0 };
	ToBCDTime(tTime, log.vTime);
	log.cType = cType;
	buf.append((const char*) &log, sizeof(log));
	return buf;
}
