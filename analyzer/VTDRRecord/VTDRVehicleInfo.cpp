/*
 * VTDRVehicleInfo.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRVehicleInfo.h"

VTDRVehicleInfo::VTDRVehicleInfo()
{
	cDataCode = VTDRRecord::VehicleInfo;
}

VTDRVehicleInfo::~VTDRVehicleInfo()
{
}

int VTDRVehicleInfo::Read(const char* buf)
{
	VehicleInfo* ptrInfo = (VehicleInfo*) buf;
	//FIXME: shall gb2312->utf8
	strTypeCode.assign((const char*) ptrInfo->TypeCode,
			min(sizeof(ptrInfo->TypeCode),
					strlen((const char*) ptrInfo->TypeCode)));
	if (strTypeCode.size())
	{
		char buf[64] =
		{ 0 };
		gb2312toutf8(strTypeCode.c_str(), strTypeCode.size(), buf, 64);
		strTypeCode = buf;
	}
	strPlateNumber.assign((const char*) ptrInfo->PlateNumber,
			min(sizeof(ptrInfo->PlateNumber),
					strlen((const char*) ptrInfo->PlateNumber)));
	if (strPlateNumber.size())
	{
		char buf[64] =
		{ 0 };
		gb2312toutf8(strPlateNumber.c_str(), strPlateNumber.size(), buf, 64);
		strPlateNumber = buf;
	}
	strPlateClass.assign((const char*) ptrInfo->PlateClass,
			min(sizeof(ptrInfo->PlateClass),
					strlen((const char*) ptrInfo->PlateClass)));
	if (strPlateClass.size())
	{
		char buf[64] =
		{ 0 };
		gb2312toutf8(strPlateClass.c_str(), strPlateClass.size(), buf, 64);
		strPlateClass = buf;
	}
	return sizeof(*ptrInfo);
}

string& VTDRVehicleInfo::Write(string& buf)
{
	VehicleInfo info;
	memset(&info, 0, sizeof(info));
	memcpy(info.TypeCode, strTypeCode.data(),
			min(sizeof(info.TypeCode), strTypeCode.size()));
	memcpy(info.PlateNumber, strPlateNumber.data(),
			min(sizeof(info.PlateNumber), strPlateNumber.size()));
	memcpy(info.PlateClass, strPlateClass.data(),
			min(sizeof(info.PlateClass), strPlateClass.size()));
	buf.append((const char *) &info, sizeof(info));
	return buf;
}

string& VTDRVehicleInfo::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << endl;
	stream << "TypeCode:" << strTypeCode << endl;
	stream << "PlateNo.:" << strPlateNumber << endl;
	stream << "PlateClass:" << strPlateClass << endl;
	return buf = stream.str();
}
