/*
 * VTDRRealTime.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRREALTIME_H_
#define VTDRREALTIME_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.4
class VTDRRealTime:public VTDRRecord
{
public:
	VTDRRealTime();
	virtual ~VTDRRealTime();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tTime;
};

#endif /* VTDRREALTIME_H_ */
