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
#include <stdio.h>
#include <time.h>

Protocol::Protocol()
{
  bOTA = false;
}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
}

int Protocol::Read(Packet::CmdWord cmd,int wait_ms,RS232Port & port,Packet& packet)
{
  const char* c;
  int n;
  if (bOTA)
    {
      c = (const char*)&cmd;
      n = 1;
    }
  else
    {
      packet.SetCmdPacket(cmd);
      n = packet.GetSize();
      c = packet.GetData();
    }
  if (n == port.Write(c, n))
    {
      packet.ReceiveFrameFrom(port,wait_ms);
      packet.Dump();
      return packet.GetSize();
    }
  return -1;
}




