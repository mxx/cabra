/*
 * VTDRSpeedStatusLog.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRSPEEDSTATUSLOG_H_
#define VTDRSPEEDSTATUSLOG_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012 A. 2.17
class VTDRSpeedStatusLog: public VTDRRecord
{
public:
	enum eSpeedStatus
	{
		Normal = 0x01,Abnormal
	};

	VTDRSpeedStatusLog();
	virtual ~VTDRSpeedStatusLog();
	int Read(const char* buf);
	string& Write(string& buf);
	eSpeedStatus Status;
	time_t tStart;
	time_t tEnd;
	int recSpeed[60];
	int refSpeed[60];
protected:
	// Table A.32


	typedef struct _SpeedStatusLog
	{
		unsigned char SpeedStatus;
		VTDRTime StartTime;
		VTDRTime EndTime;
		struct RecordSpeed
		{
			unsigned char recSpeed;
			unsigned char refSpeed;
		} __attribute__ ((packed)) Speeds[60];
	} __attribute__ ((packed)) SpeedStatusLog;
};

#endif /* VTDRSPEEDSTATUSLOG_H_ */
