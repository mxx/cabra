/*
 * VTDROvertimeDriveRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDROVERTIMEDRIVERECORD_H_
#define VTDROVERTIMEDRIVERECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.13
class VTDROvertimeDriveRecord:public VTDRRecord
{
public:
	VTDROvertimeDriveRecord();
	virtual ~VTDROvertimeDriveRecord();
	int Read(const char* buf);
	string& Write(string& buf);
	string strLicese;
	time_t tStartTime;
	time_t tEndTime;
	float startLongititude;
	float startLatitude;
	float endLogititude;
	float endLatitude;
	int startAltitude;
	int endAltitude;

protected:
	typedef struct _OvertimeDriveRecord
	{
		unsigned char DriverLicense[18];
		VTDRTime startTime;
		VTDRTime endTime;
		Position startPos;
		Position endPos;
	} __attribute__ ((packed)) OvertimeDriveRecord ;
};

#endif /* VTDROVERTIMEDRIVERECORD_H_ */
