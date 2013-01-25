/*
 * VTDRVehicleConfigure.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRVEHICLECONFIGURE_H_
#define VTDRVEHICLECONFIGURE_H_
#include "VTDRRecord.h"
#include <string>
using namespace std;
class VTDRVehicleConfigure: public VTDRRecord
{
public:
	VTDRVehicleConfigure();
	virtual ~VTDRVehicleConfigure();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	string strNameOf[8];
	time_t tTime;
	char cState;
protected:
	typedef struct _Config
	{
		VTDRTime vTime;
		char cStataByte;
		unsigned char NameOfD[8][10];
	}__attribute__ ((packed)) Config;
};

#endif /* VTDRVEHICLECONFIGURE_H_ */
