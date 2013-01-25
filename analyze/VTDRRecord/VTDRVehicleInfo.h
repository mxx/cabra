/*
 * VTDRVehicleInfo.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRVEHICLEINFO_H_
#define VTDRVEHICLEINFO_H_
#include <string>
#include "VTDRRecord.h"
using namespace std;
class VTDRVehicleInfo: public VTDRRecord
{
public:
	VTDRVehicleInfo();
	virtual ~VTDRVehicleInfo();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	string strTypeCode;
	string strPlateNumber;
	string strPlateClass;
protected:
	typedef struct _VehicleInfo
	{
		unsigned char TypeCode[17];
		unsigned char PlateNumber[12];
		unsigned char PlateClass[12];
	}__attribute__ ((packed)) VehicleInfo;

};

#endif /* VTDRVEHICLEINFO_H_ */
