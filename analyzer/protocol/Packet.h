/*
 * Packet.h
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */

#ifndef PACKET_H_
#define PACKET_H_
#include <string>
#include "RS232Port.h"

using namespace std;
struct PacketHead
{
        unsigned char cTag[2];
        unsigned char cCmdWord;
        unsigned char Len[2];
        unsigned char dummy;
}  __attribute__ ((packed));

typedef struct
{
    unsigned char Speed;
    unsigned char SwitchState;
} __attribute__ ((packed)) BeforeStopState;

struct AccidentData {
        unsigned char bcdYear;
        unsigned char bcdMonth;
        unsigned char bcdDay;
        unsigned char bcdHour;
        unsigned char bcdMinute;
        unsigned char bcdSecond;
        BeforeStopState State[100];
}  __attribute__ ((packed));

struct RealTime {
                unsigned char bcdYear;
                unsigned char bcdMonth;
                unsigned char bcdDay;
                unsigned char bcdHour;
                unsigned char bcdMinute;
                unsigned char bcdSecond;
}  __attribute__ ((packed));

struct RecordTime {
                        unsigned char bcdYear;
                        unsigned char bcdMonth;
                        unsigned char bcdDay;
                        unsigned char bcdHour;
                        unsigned char bcdMinute;
        }  __attribute__ ((packed));

struct VehicleFeature
{
        unsigned char FeatureHi;
        unsigned char FeatureM;
        unsigned char FeatureLo;
}  __attribute__ ((packed));

struct VINNumberClass
{
        char VIN[17];
        char Number[9];
        char Class[12];
} __attribute__ ((packed));

struct Mileage {
        unsigned char bcd10Mileage;
        unsigned char bcdYear;
        unsigned char bcdMonth;
        unsigned char bcdDay;
        unsigned char bcdHour;
        unsigned char bcdMinute;
        unsigned char bcdSecond;
} __attribute__ ((packed));

struct DriverInfo
{
        unsigned char DriverCode[3];
        char LicenceNumber[18];
} __attribute__ ((packed));

struct OverWorkRecord
{
        char LicenceNumber[18];
        struct RecordTime start;
        struct RecordTime end;
} __attribute__ ((packed));

struct SpeedRecord
{
        struct RecordTime startTime;
        unsigned char speed[60];
} __attribute__ ((packed));


class Packet
{
public:
	enum CmdWord
	{
              CMD_OVER=0,
		GET_DriverID_LicenceID = 0x01,
		GET_RealTime,
		GET_360Hour_Mileage,
		GET_Vehicle_Feature,
		GET_360Hour_Speed,
		GET_Vehicle_VIN_Number_Class,
		GET_Accident_Data,
		GET_2Day_Mileage,
		GET_2Day_Speed,
		GET_2Day_OverDrive = 0x11,
		GET_ALL_PARA =  0x14,
		SET_DriverID_LicenceID = 0x81,
		SET_Vehicle_VIC_Number_Class,
		SET_Time = 0xC2,
		SET_Vehicle_Feature = 0xC3,
		GET_CMD_ERROR = 0xFA,
		SET_CMD_ERROR = 0xFB,
		CMD_RESET = 0xFF
	};


	Packet();
	virtual ~Packet();
	const char* GetData() { return data.data();}
	int GetSize() {return data.size();}
	void SetCmdPacket(CmdWord cmd);
	void ReceiveFrameFrom(RS232Port & port,int wait_ms=10000);
	int GetDriverCode(void);
	string GetLicenseID(void);
	struct AccidentData*  GetAccidentData(unsigned int& num);
	struct SpeedRecord* GetSpeedData(unsigned int& num);
	void Dump();
protected:
	enum PacketState
	{
		NONE = 0,
		SOF1,
		SOF2,
		CMD,
		SIZE1,
		SIZE2,
		DUMMY,
		DATA,
		CHECKSUM
	} frameState;
	unsigned char XOR();
	string data;
};

#endif /* PACKET_H_ */
