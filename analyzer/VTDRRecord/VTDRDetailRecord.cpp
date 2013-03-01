/*
 * VTDRDetailRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRDetailRecord.h"

VTDRDetailRecord::VTDRDetailRecord() :
		tEnd(0), Longititude(0.0), Latitude(0.0), Altitude(0)

{
	cDataCode = AccidentSuspectPoint;
}

VTDRDetailRecord::~VTDRDetailRecord()
{
}

int VTDRDetailRecord::Read(const char* buf)
{
	DetailRecord* ptrRec = (DetailRecord*) buf;
	tEnd = ToSystime(ptrRec->endTime);
	strLicenseNumber.assign((const char*)(ptrRec->License),sizeof(ptrRec->License));
	for (int i = 0; i < 100; i++)
	{
		Speed[i] = ptrRec->record[i].speed;
		State[i] = ptrRec->record[i].state;
	}
	readPosition(ptrRec->position, Longititude, Latitude, Altitude);
	return sizeof(*ptrRec);
}

string& VTDRDetailRecord::Write(string& buf)
{
	DetailRecord rec;
	ToBCDTime(tEnd, rec.endTime);
	SET(rec.License, strLicenseNumber);
	for (int i = 0; i < 100; i++)
	{
		rec.record[i].speed = Speed[i];
		rec.record[i].state = State[i];
	}
	writePosition(rec.position, Longititude, Latitude, Altitude);
	buf.append((const char*) &rec, sizeof(rec));
	return buf;
}

string& VTDRDetailRecord::Dump(string& buf)
{
	stringstream s(buf);
	s << VTDRRecord::Dump(buf) << endl;
	s << "Driver License:" << strLicenseNumber.c_str() << endl;
	s << "Last End Time:" << ctime(&tEnd);
	s << "Time\tSpeed\tState:" << endl;

	for (int i = 0; i < 100; i++)
	{
		s << i/5 << "." << i%5*2 << "\t" << Speed[i] << "\t" << State[i] << endl;
	}

	return buf += s.str();
}

