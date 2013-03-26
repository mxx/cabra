/*
 * Packet.h
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */

#ifndef PACKET_H_
#define PACKET_H_
#include <string>

using namespace std;

enum CmdWord
{
	GET_STD_VERSION = 0,
	GET_Driver_LicenceID,
	GET_RealTime,
	GET_Odometer,
	GET_Pulse_Param,
	GET_Vehicle_Info,
	GET_State_Config,
	GET_Uniq_No,
	GET_Speed_Record,
	GET_Postion_Record,
	GET_Suspious_Record = 0x10,
	GET_OverDrive_Record,
	GET_Driver_Record,
	GET_Power_Record,
	GET_Param_Record,
	GET_Speed_State_Log,
	SET_Vehicle_Info = 0x82,
	SET_Install_Date,
	SET_Param_Config,
	SET_Clock = 0xC2,
	SET_Pulse_Param,
	SET_Odometer,
	CHECK_Enter = 0xE0,
	CHECK_Input_Speed_Pulse,
	CHECK_Output_Speed_Pulse,
	CHECK_Output_RTC_Pulse,
	CHECK_Leave,
	GET_ERROR = 0xFA,
	SET_ERROR = 0xFB,
	CMD_OVER
};

class Packet
{
public:


#ifdef _WIN32
#pragma pack(1)
#define PACK
#else
#define PACK  __attribute__ ((packed))
#endif

	typedef struct _PacketHead
	{
		unsigned char cTag[2];
		unsigned char cCmdWord;
		unsigned char Len[2];
		unsigned char dummy;
	}PACK PackHead;

#ifdef _WIN32
#pragma pack()
#endif

	Packet();
	virtual ~Packet();
	const string& GetData()
	{
		return packet;
	};
	void SetCmdPacket(CmdWord cmd,string& content);
	const string& Extract(string& buf);
	const string& GetContent()
	{
		return data;
	};
	CmdWord GetCmd()
	{
		return cmd;
	};
	void Dump();
protected:

	unsigned char get_xor(const char* data, int size);
	string::size_type posFrameStart;
	CmdWord cmd;
	int nDataSize;
	string data;
	string packet;
};

#endif /* PACKET_H_ */
