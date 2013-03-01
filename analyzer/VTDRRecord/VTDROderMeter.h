/*
 * VTDROderMeter.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRODERMETER_H_
#define VTDRODERMETER_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.5
class VTDROderMeter:public VTDRRecord
{
public:
	VTDROderMeter();
	virtual ~VTDROderMeter();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tNow;
	time_t tInstall;
	float startMeter;
	float sumMeter;
protected:
	typedef struct _OderMeter
		{
			VTDRTime vTime;
			VTDRTime installTime;
			unsigned char startValue[4];
			unsigned char oderMeter[4];
		}__attribute__ ((packed)) OderMeter;


};

#endif /* VTDRODERMETER_H_ */
