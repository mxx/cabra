/*
 * VTDRPulseModulus.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRPulseModulus.h"

VTDRPulseModulus::VTDRPulseModulus() :
		tTime(0), sModulus(0)
{
	cDataCode = VTDRRecord::PulseModulu;
}

VTDRPulseModulus::~VTDRPulseModulus()
{
}

int VTDRPulseModulus::Read(const char* buf)
{
	PulseModulus* ptrMod = (PulseModulus*) buf;
	tTime = ToSystime(ptrMod->vTime);
	sModulus = ((ptrMod->cHighByte << 8) & 0xFF00) + ptrMod->cLowByet;
	return sizeof(*ptrMod);
}

string& VTDRPulseModulus::Write(string& buf)
{
	PulseModulus mod =
	{ 0 };
	ToBCDTime(tTime, mod.vTime);
	mod.cHighByte = (unsigned char) ((sModulus >> 8) & 0x0FF);
	mod.cLowByet = (unsigned char) (sModulus & 0x0FF);
	buf.append((const char*) &mod, sizeof(mod));
	return buf;
}

string& VTDRPulseModulus::Dump(string& buf)
{
	stringstream stream;
	stream << VTDRRecord::Dump(buf) << " Time:" << ctime(&tTime);
	stream << "param:" << sModulus;

	return buf = stream.str();
}
