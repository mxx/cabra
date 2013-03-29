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


#include "Packet.h"
Packet::Packet() :
		posFrameStart(string::npos)
{


}

Packet::~Packet()
{

}

void Packet::SetCmdPacket(CmdWord cmd,string& content)
{
	data = content;
	this->cmd = cmd;
	packet = (char) 0xAA;
	packet += 0x75;
	packet += cmd;
	short len = (short)data.size();
	packet.append(1,(len/256)&0x0FF);
	packet.append(1,len%256);
	packet.append(1,0); //dummy word
	packet.append(data);
	packet += get_xor(packet.data(),packet.size());
}

unsigned char Packet::get_xor(const char* data, int size)
{
	unsigned char sum = 0;
	int i;
	for (i = 0; i < size; i++)
	{
		sum ^= data[i];
	}
	return sum;
}

const string&  Packet::Extract(string& buf)
{

	PackHead* ptrPacket = NULL;
	do
	{
		if (posFrameStart == string::npos)
			posFrameStart = buf.find("\0x55\0x7A");

		if (posFrameStart != string::npos)
		{
			PackHead* ptrPacket = (PackHead*)(buf.data() + posFrameStart);
			if (buf.size() < posFrameStart + sizeof(ptrPacket->dummy))
				break;

			int data_size = ptrPacket->Len[0] * 256 + ptrPacket->Len[1];
			if (buf.size() < posFrameStart + sizeof(ptrPacket->dummy) + data_size)
				break;

			if (get_xor(buf.data()+posFrameStart,sizeof(*ptrPacket)+data_size+1)==0)
			{
				cmd = (CmdWord)ptrPacket->cCmdWord;
				nDataSize = data_size;
				data = buf.substr(posFrameStart+sizeof(PackHead),data_size);
				buf.erase(0,posFrameStart + data_size + sizeof(*ptrPacket) +1);
				break;
			}
			else
			{
				buf.erase(posFrameStart,2);
				posFrameStart = string::npos;
				continue;
			}

		}
		else
		{
			buf.erase(0, buf.size()); //buffer clear,for compatible win win32
			break;
		}
	} while (buf.size() > sizeof(ptrPacket->cTag));
	return data;
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

