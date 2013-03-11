/*
 * Protocol.h
 *
 *  Created on: 2010-4-16
 *      Author: mxx
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Packet.h"
#include "RS232Port.h"
class Protocol
{
public:
	Protocol();
	virtual ~Protocol();
	int ReadDriverIDLicenceID(RS232Port & port,Packet& packet);
	int ReadRealTime(RS232Port & port,Packet& packet);
       int Read360HourMileage(RS232Port & port,Packet& packet);
	int ReadVehicleFeature(RS232Port & port,Packet& packet);
	int Read360HourSpeed(RS232Port & port,Packet& packet);
	int ReadVehicleVIN_Number_Class(RS232Port & port,Packet& packet);
	int ReadAccident_Data(RS232Port & port,Packet& packet);
	int Read2DayMileage(RS232Port & port,Packet& packet);
	int Read2DaySpeed(RS232Port & port,Packet& packet);
	int Read2DayOverDrive(RS232Port & port,Packet& packet);
	int ReadAllPara(RS232Port & port,Packet& packet);
	void Over(RS232Port & port);
	int Reset(RS232Port & port);
	void SetOTA(bool b) {bOTA = b;};
protected:
	int Read(Packet::CmdWord cmd,int wait_ms,RS232Port & port,Packet& packet);
	bool bOTA;
};


#endif /* PROTOCOL_H_ */
