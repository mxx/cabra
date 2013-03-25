/*
 * Protocol.h
 *
 *  Created on: 2010-4-16
 *      Author: mxx
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Packet.h"
#include "VTDRRecord.h"
class Protocol
{
public:
	Protocol();
	virtual ~Protocol();
	VTDRRecord* Parse(Packet& packet);
	Packet Command(CmdWord cmd,time_t tStart, time_t tEnd, int size);
protected:


};


#endif /* PROTOCOL_H_ */
