/*
 * VTDRVehicleConfigure.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRVehicleConfigure.h"

VTDRVehicleConfigure::VTDRVehicleConfigure() :
		tTime(0), cState(0)
{
	cDataCode = StateConfig;
}

VTDRVehicleConfigure::~VTDRVehicleConfigure()
{

}

int VTDRVehicleConfigure::Read(const char* buf)
{
	Config* ptrConfig = (Config*) buf;
	tTime = ToSystime(ptrConfig->vTime);
	cState = ptrConfig->cStataByte;
	for (int i = 0; i < 8; i++)
	{
		char buf[32] =
		{ 0 };
		strNameOf[i].assign((const char*) ptrConfig->NameOfD[i],
				sizeof(ptrConfig->NameOfD[i]));
		if (strNameOf[i].size())
		{
			gb2312toutf8(strNameOf[i].c_str(), strNameOf[i].size(), buf, 32);
			strNameOf[i] = buf;
		}
	}
	return sizeof(*ptrConfig);
}

string& VTDRVehicleConfigure::Write(string& buf)
{
	Config conf =
	{ 0 };
	conf.cStataByte = cState;
	ToBCDTime(tTime, conf.vTime);
	for (int i = 0; i < 8; i++)
	{
		if (strNameOf[i].empty())
			continue;
		utf8togb2312(strNameOf[i].c_str(), strNameOf[i].size(), (char*)conf.NameOfD[i], 10);
	}

	buf.append((const char*) &conf, sizeof(conf));
	return buf;
}

string& VTDRVehicleConfigure::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << " Time:" << ctime(&tTime);
	stream << "State:";
	stream << hex << uppercase << (int)(cState&0x000000FF) << endl;
	for (int i = 0; i < 8; i++)
	{
		stream << i << ":" << strNameOf[i] << endl;
	}
	return buf = stream.str();
}
