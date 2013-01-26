/*
 * VTDRUniqCode.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRUNIQCODE_H_
#define VTDRUNIQCODE_H_
#include "VTDRRecord.h"
#include <string>
using namespace std;
class VTDRUniqCode : public VTDRRecord
{
public:
	VTDRUniqCode();
	virtual ~VTDRUniqCode();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	string strManufacture;
	string strAuthType;
	int nYear;
	int nMonth;
	int nDay;
	unsigned int nSerialNumber;
protected:
	typedef struct _UniqCode
	{
		unsigned char Manufacture[7];
		unsigned char AuthrizedType[16];
		unsigned char bcdYear;
		unsigned char bcdMonth;
		unsigned char bcdDay;
		unsigned int  SerialNumber;
		unsigned char reserved[5];
	}__attribute__ ((packed)) UniqCode;
};

#endif /* VTDRUNIQCODE_H_ */
