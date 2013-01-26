/*
 * VTDRPulseModulus.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRPULSEMODULUS_H_
#define VTDRPULSEMODULUS_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.6
class VTDRPulseModulus: public VTDRRecord
{
public:
	VTDRPulseModulus();
	virtual ~VTDRPulseModulus();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tTime;
	unsigned short sModulus;
protected:
	typedef struct _PulseModulus
	{
		VTDRTime vTime;
		unsigned char cHighByte;
		unsigned char cLowByet;
	}__attribute__ ((packed)) PulseModulus;
};

#endif /* VTDRPULSEMODULUS_H_ */
