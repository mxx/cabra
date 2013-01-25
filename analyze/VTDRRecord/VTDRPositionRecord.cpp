/*
 * VTDRPositionRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */
#include <arpa/inet.h>
#include "VTDRPositionRecord.h"

VTDRPositionRecord::VTDRPositionRecord() :
		tStart(0)
{
	cDataCode = VTDRRecord::PositionRecord;
}

VTDRPositionRecord::~VTDRPositionRecord()
{

}

int VTDRPositionRecord::Read(const char* buf)
{
	PositionRecord* ptrRec = (PositionRecord*) buf;
	tStart = ToSystime(ptrRec->vStart);
	for (int i = 0; i < 60; i++)
	{
		readPosition(ptrRec->record[i].pos, Longititude[i], Latitude[i],
				Altitude[i]);
		Speed[i] = ptrRec->record[i].speed;
	}
	return sizeof(*ptrRec);
}

string& VTDRPositionRecord::Write(string& buf)
{
	PositionRecord rec =
	{ 0xFF };

	ToBCDTime(tStart, rec.vStart);
	for (int i = 0; i < 60; i++)
	{
		writePosition(rec.record[i].pos, Longititude[i], Latitude[i],
				Altitude[i]);
		rec.record[i].speed = (unsigned char) (Speed[i] & 0x0FF);
	}
	buf.append((const char*) &rec, sizeof(rec));
	return buf;
}
