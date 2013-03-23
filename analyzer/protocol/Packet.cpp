/*
 * Packet.cpp
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */

#include <time.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/time.h>
#else
#include "stdafx.h"
#include <Windows.h>

#endif

#define TRACE(args)

#include "Packet.h"
Packet::Packet() :
		posFrameStart(string::npos)
{
	frameState = NONE;
	nFrameIndex = 0;
	cmd = CMD_OVER;
	nDataSize = 0;
}

Packet::~Packet()
{
	// TODO Auto-generated destructor stub
}

void Packet::SetCmdPacket(CmdWord cmd)
{
//      Full Packet

	data = (char) 0xAA;
	data += 0x75;
	data += cmd;
	data.append(3, 0);
	data += xor();
}

int Packet::GetDriverCode(void)
{
	struct PacketHead *Head = (struct PacketHead *) GetData();
	if (Head->cCmdWord == GET_DriverID_LicenceID)
	{
		struct DriverInfo* info = (struct DriverInfo*) (Head + 1);
		return ((info->DriverCode[0] << 16) & 0xFF0000)
				| ((info->DriverCode[1] << 8) & 0xFF00) | info->DriverCode[2];
	}
	return -1;
}

string Packet::GetLicenseID(void)
{
	struct PacketHead *Head = (struct PacketHead *) GetData();

	if (Head->cCmdWord == GET_DriverID_LicenceID)
	{
		struct DriverInfo* info = (struct DriverInfo*) (Head + 1);
		string id(info->LicenceNumber, 18);
		return id;
	}
	return "";
}

struct AccidentData* Packet::GetAccidentData(unsigned int& num)
{
	struct PacketHead *Head = (struct PacketHead *) GetData();
	num = 0;
	if (Head->cCmdWord == GET_Accident_Data)
	{
		int n = ((Head->Len[0] << 8) & 0xFF00) | (Head->Len[1]);
		num = n / sizeof(struct AccidentData);
		return (struct AccidentData*) (Head + 1);
	}
	return NULL;
}

struct SpeedRecord* Packet::GetSpeedData(unsigned int& num)
{
	struct PacketHead *Head = (struct PacketHead *) GetData();
	num = 0;
	if (Head->cCmdWord == GET_360Hour_Speed || Head->cCmdWord == GET_2Day_Speed)
	{
		unsigned int n = ((Head->Len[0] << 8) & 0xFF00) | (Head->Len[1]);
		num = n;
		return (struct SpeedRecord*) (Head + 1);
	}
	return NULL;
}

unsigned char Packet::get_xor(string& data)
{
	string::iterator it;
	unsigned char sum = 0;
	string str = data.substr(posFrameStart,nDataSize+6);
	it = data.begin();
	sum = *it;
	it++;
	for (; it < data.end(); it++)
	{

		sum ^= *it;
	}
	return sum;
}

const char* Packet::Extract(string& buf)
{
	do
	{
		switch (frameState)
		{
		case NONE:
			posFrameStart = buf.find("\0x55\0x7A");
			if (posFrameStart != string::npos)
			{
				frameState = SOF;
				nFrameIndex = 2;
			}
			else
			{
				buf.erase(0,buf.size()); //buf clear,for compatible win win32
				return NULL;
			}
			break;
		case SOF:
			frameState = CMD;
			nFrameIndex = 3;
			break;
		case CMD:
			cmd = (CmdWord)buf[posFrameStart + 2];
			frameState = SIZE;
			nFrameIndex = 5;
			break;
		case SIZE:
			nDataSize = buf[posFrameStart + 3] * 256 + buf[posFrameStart + 4];
			frameState = DUMMY;
			nFrameIndex = 6;
			break;
		case DUMMY:
			if (!nDataSize)
			{
				frameState = CHECKSUM;
				nFrameIndex = 7;
			}
			else
			{
				frameState = DATA;
				nFrameIndex = nDataSize + 6;
			}
			break;
		case DATA:
			if (buf.size()>(posFrameStart + nFrameIndex -1))
			{
				frameState = CHECKSUM;
				nFrameIndex = nDataSize + 7;
			}
			break;
		case CHECKSUM:
			if (get_xor(buf)==0)
			{
				data = buf.substr(posFrameStart+6,nDataSize);
				buf.erase(0,posFrameStart+nDataSize + 7);
				return data.data();
			}
			else
			{
				buf.erase(0, posFrameStart+2);
				frameState = NONE;
				nFrameIndex = 1;
				posFrameStart = 0;
			}
			break;
		};
	} while (buf.size() > (posFrameStart + nFrameIndex - 1));
	return NULL;
}

void Packet::Dump()
{
	char buf[1024] =
	{ 0 };
	int n = 0;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		sprintf(buf + n, "%02hhX ", data[i]);
		n = strlen(buf);
		if (n > 1000)
			break;
	}

}

