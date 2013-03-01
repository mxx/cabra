/*
 * VTDRPositionRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRPOSITIONRECORD_H_
#define VTDRPOSITIONRECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.11
class VTDRPositionRecord: public VTDRRecord
{
public:
	VTDRPositionRecord();
	virtual ~VTDRPositionRecord();
	time_t tStart;
	float Longititude[60]; //unit: minute
	float Latitude[60];
	int Altitude[60];
	int Speed[60];
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
protected:

	typedef struct _PositionRecord
	{
		VTDRTime vStart;
		struct {
			Position pos;
			unsigned char speed;
		} __attribute__ ((packed)) record[60];
	} __attribute__ ((packed)) PositionRecord;
};

#endif /* VTDRPOSITIONRECORD_H_ */
