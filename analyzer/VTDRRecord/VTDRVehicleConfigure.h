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
	}PACK Config;
};

class VTDRVehicleConfigureSetup: public VTDRVehicleConfigure
{
public:
	VTDRVehicleConfigureSetup();
	virtual ~VTDRVehicleConfigureSetup();

protected:
	typedef struct _ConfigSetup
	{
		unsigned char NameOfD[8][10];
	}PACK ConfigSetup;
public:

	string& Write(string& buf)
	{
		ConfigSetup conf =
		{ 0 };
		for (int i = 0; i < 8; i++)
		{
			if (strNameOf[i].empty())
				continue;
			utf8togb2312(strNameOf[i].c_str(), strNameOf[i].size(), (char*)conf.NameOfD[i], 10);
		}

		buf.append((const char*) &conf, sizeof(conf));
		return buf;
	};


};

#endif /* VTDRVEHICLECONFIGURE_H_ */
