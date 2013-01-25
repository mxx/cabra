/*
 * VTDROvertimeDriveRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDROvertimeDriveRecord.h"

VTDROvertimeDriveRecord::VTDROvertimeDriveRecord() :
		tStartTime(0), tEndTime(0), startLongititude(0.0), startLatitude(0.0), startAltitude(
				0), endLogititude(0.0), endLatitude(0.0), endAltitude(0)
{
	cDataCode = OverTimeDriving;
}

VTDROvertimeDriveRecord::~VTDROvertimeDriveRecord()
{

}

int VTDROvertimeDriveRecord::Read(const char* buf)
{
	OvertimeDriveRecord* ptrRec = (OvertimeDriveRecord*) buf;
	ASSIGN(strLicese, ptrRec->DriverLicense);
	tStartTime = ToSystime(ptrRec->startTime);
	tEndTime = ToSystime(ptrRec->endTime);
	readPosition(ptrRec->startPos,startLongititude,startLatitude,startAltitude);
	readPosition(ptrRec->endPos,endLogititude,endLatitude,endAltitude);
	return sizeof(*ptrRec);
}

string& VTDROvertimeDriveRecord::Write(string& buf)
{
	OvertimeDriveRecord rec =
	{ 0 };
	SET(rec.DriverLicense, strLicese);
	ToBCDTime(tStartTime, rec.startTime);
	ToBCDTime(tEndTime, rec.endTime);
	writePosition(rec.startPos,startLongititude,startLatitude,startAltitude);
	writePosition(rec.endPos,endLogititude,endLatitude,endAltitude);
	buf.append((const char*)&rec,sizeof(rec));
	return buf;
}
