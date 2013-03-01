/*
 * VTDRDetailRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRDETAILRECORD_H_
#define VTDRDETAILRECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.12
using namespace std;
class VTDRDetailRecord:public VTDRRecord
{
public:
	VTDRDetailRecord();
	virtual ~VTDRDetailRecord();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);

	time_t tEnd;
	string strLicenseNumber;
	int Speed[100];
	int State[100];
	float Longititude; //unit: minute
	float Latitude;
	int Altitude;
protected:
	typedef struct _DetailDRecord
	{
		VTDRTime endTime;
		unsigned char License[18];
		struct
		{
			unsigned char speed;
			unsigned char state;
		} __attribute__ ((packed)) record[100];
		Position position;
	} __attribute__ ((packed)) DetailRecord;
};

#endif /* VTDRDETAILRECORD_H_ */
