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
#include <list>
using namespace std;
class Protocol
{
public:
	
	Protocol();
	virtual ~Protocol();
	int Parse(Packet& packet, list<VTDRRecord*>& recs);
	Packet Command(CmdWord cmd,time_t tStart, time_t tEnd, int size);
    Packet Command(VTDRRecord* ptrRec);
protected:


};


#endif /* PROTOCOL_H_ */
