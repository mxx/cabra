/*
 * VTDROderMeter.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRODERMETER_H_
#define VTDRODERMETER_H_
#include <string>
#include "VTDRRecord.h"


//GB/T 19056-2012  A.2.5
class VTDROderMeter:public VTDRRecord
{
public:
	VTDROderMeter();
	virtual ~VTDROderMeter();
	int Read(const char* buf);
	string& Write(string& buf);
	string& Dump(string& buf);
	time_t tNow;
	time_t tInstall;
	double startMeter;
	double sumMeter;
	int sumMeterRaw;
protected:
	typedef struct _OderMeter
		{
			VTDRTime vTime;
			VTDRTime installTime;
			unsigned char startValue[4];
			unsigned char oderMeter[4];
		}PACK OderMeter;


};

class VTDROderMeterInit:public VTDROderMeter
{
public:
    VTDROderMeterInit()
    {
    };
    virtual ~VTDROderMeterInit()
    {
    };
    
    string& Write(string& buf)	
    {
        unsigned char data[4]={0};
        int v = (int)(startMeter*10.0);
        for (int i = 0; i < sizeof(data); i++)
        {
            data[sizeof(data) - i - 1] = INT2BCDchar( v % 100);
            v = v / 100;
            if (v == 0)
                break;
        };
        buf.assign((const char*)data,sizeof(data));
        return buf;
	};
};
#endif /* VTDRODERMETER_H_ */
