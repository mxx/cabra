/*
 * VTDRDriverIDRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRDriverIDRecord.h"

const char* VTDRDriverIDRecord::decodeType[] =
{ "Unknown", "Login", "Logout" };

VTDRDriverIDRecord::VTDRDriverIDRecord() :
		tStart(0), cType(0)
{
	cDataCode = VTDRRecord::DriverInfo;
}

VTDRDriverIDRecord::~VTDRDriverIDRecord()
{

}

int VTDRDriverIDRecord::Read(const char* buf)
{
	DriverIDRecord* ptrRec = (DriverIDRecord*) buf;
	tStart = ToSystime(ptrRec->startTime);
	ASSIGN(strLicenseNumber, ptrRec->License);
	cType = ptrRec->cType;
	return sizeof(*ptrRec);
}

string& VTDRDriverIDRecord::Write(string& buf)
{
	DriverIDRecord rec =
	{ 0 };
	ToBCDTime(tStart, rec.startTime);
	SET(rec.License, strLicenseNumber);
	rec.cType = cType;
	buf.append((const char*) &rec, sizeof(rec));
	return buf;
}

string& VTDRDriverIDRecord::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << endl;
	stream <<"RecTime:" << Time2String(tStart) << endl;
	stream << "DriverLicense:" << strLicenseNumber.c_str();
	stream << ",RecType:" << decodeType[mapType(cType)] << "(" << (int)cType << ")" << endl;
	return buf = stream.str();
}
