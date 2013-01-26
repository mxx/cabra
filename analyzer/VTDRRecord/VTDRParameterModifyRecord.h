/*
 * VTDRParameterModifyRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRPARAMETERMODIFYRECORD_H_
#define VTDRPARAMETERMODIFYRECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.16
class VTDRParameterModifyRecord:public VTDRRecord
{
public:
	VTDRParameterModifyRecord();
	virtual ~VTDRParameterModifyRecord();
	int Read(const char* buf);
	string& Write(string& buf);
	time_t tTime;
	unsigned char cType;
protected:
	typedef struct _ModifyLog
	{
		VTDRTime vTime;
		unsigned char cType;
	} __attribute__ ((packed)) ModifyLog ;
};

#endif /* VTDRPARAMETERMODIFYRECORD_H_ */
