/*
 * VTDROderMeter.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDROderMeter.h"


VTDROderMeter::VTDROderMeter() :
		tNow(0), tInstall(0), startMeter(0.0), sumMeter(0.0)
{
	cDataCode = VTDRRecord::OderMeter;
}

VTDROderMeter::~VTDROderMeter()
{
}

int VTDROderMeter::Read(const char* buf)
{
	OderMeter* ptrMeter = (OderMeter*) buf;
	tNow = ToSystime(ptrMeter->vTime);
	tInstall = ToSystime(ptrMeter->installTime);
	startMeter = BCD2INT((const char*) (ptrMeter->startValue),
			sizeof(ptrMeter->startValue)) / 10.0;
	sumMeter = BCD2INT((const char*) (ptrMeter->oderMeter),
			sizeof(ptrMeter->oderMeter)) / 10.0;
	return sizeof(*ptrMeter);

}

string& VTDROderMeter::Write(string& buf)
{
	OderMeter meter =
	{ 0 };
	ToBCDTime(tNow, meter.vTime);
	ToBCDTime(tInstall, meter.installTime);
	int v = (int) (startMeter * 10.0);
	for (int i = 0; i < sizeof(meter.startValue); i++)
	{
		meter.startValue[sizeof(meter.startValue) - i] = INT2BCDchar(v % 100);
		v = v / 100;
		if (v == 0)
			break;
	}
	for (int i = 0; i < sizeof(meter.oderMeter); i++)
	{
		meter.oderMeter[sizeof(meter.startValue) - i] = INT2BCDchar(v % 100);
		v = v / 100;
		if (v == 0)
			break;
	}
	buf.append((const char*) &meter, sizeof(meter));
	return buf;
}

string& VTDROderMeter::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << " Time:" << ctime(&tNow);
	stream << " InstallTime:" << ctime(&tInstall);
	stream << " StartMeter:" << startMeter;
	stream << " AccumulateMeter:" << sumMeter;
	return buf = stream.str();
}
