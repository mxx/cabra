/*
 * VTDRDriverInfo.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRDRIVERINFO_H_
#define VTDRDRIVERINFO_H_
#include <string>
#include "VTDRRecord.h"
//GB/T 19056-2012  A.2.3
using namespace std;
class VTDRDriverInfo:public VTDRRecord
{
public:
	VTDRDriverInfo();
	virtual ~VTDRDriverInfo();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);

	string strLicenseNumber;
protected:
	typedef struct _DriverInfo
	{
		unsigned char license[18];
	} __attribute__ ((packed)) DriverInfo;
};

#endif /* VTDRDRIVERINFO_H_ */
