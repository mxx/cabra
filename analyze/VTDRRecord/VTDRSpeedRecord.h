/*
 * VTDRSpeedRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRSPEEDRECORD_H_
#define VTDRSPEEDRECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.10
class VTDRSpeedRecord: public VTDRRecord
{
public:
	VTDRSpeedRecord();
	virtual ~VTDRSpeedRecord();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tStart;
	int Speed[60];
	int State[60];
protected:

	typedef struct _SpeedRecord
	{
		VTDRTime startTime;
		struct SpeedState
		{
			unsigned char speed;
			unsigned char state;
		}  __attribute__ ((packed)) record[60];
	}  __attribute__ ((packed)) SpeedRecord;

};

#endif /* VTDRSPEEDRECORD_H_ */
