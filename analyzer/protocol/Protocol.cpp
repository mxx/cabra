/*
 * Protocol.cpp
 *
 *  Created on: 2010-4-16
 *      Author: mxx
 */
#ifdef _WIN32
#include "stdafx.h"
#endif
#include "Protocol.h"

#include "VTDRVersion.h"
#include "VTDRDriverInfo.h"
#include "VTDRRealTime.h"
#include "VTDROderMeter.h"
#include "VTDRPulseModulus.h"
#include "VTDRVehicleInfo.h"
#include "VTDRVehicleConfigure.h"
#include "VTDRUniqCode.h"
#include "VTDRSpeedRecord.h"
#include "VTDRPositionRecord.h"
#include "VTDRDetailRecord.h"
#include "VTDROvertimeDriveRecord.h"
#include "VTDRDriverIDRecord.h"
#include "VTDROutPoweredRecord.h"
#include "VTDRParameterModifyRecord.h"
#include "VTDRSpeedStatusLog.h"
#include <stdio.h>
#include <time.h>

Protocol::Protocol()
{

}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
}

VTDRRecord* Protocol::Parse(Packet& packet)
{
	switch(packet.GetCmd())
	{

	}
	return NULL;
}

Packet Protocol::Command(CmdWord cmd, time_t tStart, time_t tEnd, int size)
{
	VTDRTime vTime;
	string strData;
	if (tStart)
	{
		VTDRRecord::ToBCDTime(tStart,vTime);
		strData.append((const char*)&vTime,sizeof(vTime));
		VTDRRecord::ToBCDTime(tEnd,vTime);
		strData.append((const char*)&vTime,sizeof(vTime));
		unsigned short data = htons((short)size);
		strData.append((const char*) &data,sizeof(data));
	}
	Packet packet;
	packet.SetCmdPacket(cmd,strData);

	return packet;
}
