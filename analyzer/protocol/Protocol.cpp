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

int Protocol::Parse(Packet& packet, list<VTDRRecord*>& recs)
{
	VTDRRecord* ptrRec = NULL;
    int n = 0;
    int sum = 0;
    do
    {
        n++;
        switch(packet.GetCmd())
        {
        case GET_STD_VERSION:
            ptrRec = new VTDRVersion;
            break;
        case GET_Driver_LicenceID:
            ptrRec = new VTDRDriverInfo;
            break;
        case GET_RealTime:
            ptrRec = new VTDRRealTime;
            break;
        case GET_Odometer:
            ptrRec = new VTDROderMeter;
            break;
        case GET_Pulse_Param:
            ptrRec = new VTDRPulseModulus;
            break;
        case GET_Vehicle_Info:
            ptrRec = new VTDRVehicleInfo;
            break;
        case GET_State_Config:
            ptrRec = new VTDRVehicleConfigure;
            break;
        case GET_Uniq_No:
            ptrRec = new VTDRUniqCode;
            break;
        case GET_Speed_Record:
            ptrRec = new VTDRSpeedRecord;
            break;
        case GET_Postion_Record:
            ptrRec = new VTDRDriverInfo;
            break;
        case GET_Suspious_Record:
            ptrRec = new VTDRDetailRecord;
            break;
        case GET_OverDrive_Record:
            ptrRec = new VTDROvertimeDriveRecord;
            break;
        case GET_Driver_Record:
            ptrRec = new VTDRDriverIDRecord;
            break;
        case GET_Power_Record:
            ptrRec = new VTDROutPoweredRecord;
            break;
        case GET_Param_Record:
            ptrRec = new VTDRParameterModifyRecord;
            break;
        case GET_Speed_State_Log:
            ptrRec = new VTDRSpeedStatusLog;
            break;
        }
        
        if (ptrRec)
        {
            recs.push_back(ptrRec);
            sum += ptrRec->Read(packet.GetContent().data()+sum);
        }
        else
            break;
    } while(sum < packet.GetContent().size());
    return n;
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

Packet Protocol::Command(CmdWord cmd,VTDRRecord *ptrRec)
{
	Packet packet;
    int n = ptrRec->GetDataCode();
    string strData;
    ptrRec->Write(strData);
	packet.SetCmdPacket(cmd,strData);
	return packet;
}
