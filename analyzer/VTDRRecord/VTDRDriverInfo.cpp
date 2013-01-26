/*
 * VTDRDriverInfo.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRDriverInfo.h"

VTDRDriverInfo::VTDRDriverInfo()

{
	cDataCode = VTDRRecord::CurrentDriver;
}

VTDRDriverInfo::~VTDRDriverInfo()
{
	// TODO Auto-generated destructor stub
}

int VTDRDriverInfo::Read(const char* buf)
{
	DriverInfo* ptrInfo = (DriverInfo*) buf;
	ASSIGN(strLicenseNumber, ptrInfo->license);
	return sizeof(*ptrInfo);
}

string& VTDRDriverInfo::Write(string& buf)
{
	DriverInfo info =
	{ 0 };
	SET(info.license, strLicenseNumber);
	buf.append((const char*) &info, sizeof(info));
	return buf;
}

string& VTDRDriverInfo::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << " License:" << strLicenseNumber ;
	return buf = stream.str();
};
