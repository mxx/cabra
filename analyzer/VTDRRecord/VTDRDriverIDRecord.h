/*
 * VTDRDriverIDRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRDRIVERIDRECORD_H_
#define VTDRDRIVERIDRECORD_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.14
using namespace std;
class VTDRDriverIDRecord:public VTDRRecord
{
public:
	VTDRDriverIDRecord();
	virtual ~VTDRDriverIDRecord();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tStart;
	string strLicenseNumber;
	char cType;
protected:
	typedef struct _DriverIDRecord
	{
		VTDRTime startTime;
		unsigned char License[18];
		char cType;
	} PACK DriverIDRecord;
	static const char* decodeType[];
	int mapType(char n)
	{
		if (n > 2 || n < 1)
			return 0;
		return n;
	}
};

#endif /* VTDRDRIVERIDRECORD_H_ */
