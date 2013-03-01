/*
 * VTDROutPoweredRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDROUTPOWEREDRECORD_H_
#define VTDROUTPOWEREDRECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.15
class VTDROutPoweredRecord:public VTDRRecord
{
public:
	VTDROutPoweredRecord();
	virtual ~VTDROutPoweredRecord();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tTime;
	char cType;
protected:
	typedef struct _PowerLog
	{
		VTDRTime vTime;
		char cType;
	}__attribute__ ((packed)) PowerLog;

	static const char* decodeType[];
};

#endif /* VTDROUTPOWEREDRECORD_H_ */
