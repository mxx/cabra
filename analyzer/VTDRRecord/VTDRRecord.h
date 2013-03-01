/*
 * VTDRRecord.h
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#ifndef VTDRRECORD_H_
#define VTDRRECORD_H_
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>
#include <math.h>
using namespace std;

#define SET(x,y) set(x,y,sizeof(x))
#define ASSIGN(x,y) assign(x,y,sizeof(y))

typedef struct _VTDRTime
{
	unsigned char bcdYear;
	unsigned char bcdMonth;
	unsigned char bcdDay;
	unsigned char bcdHour;
	unsigned char bcdMinute;
	unsigned char bcdSecond;
}__attribute__ ((packed)) VTDRTime;

class VTDRRecord
{
public:
	enum DataCode
	{
		Version = 0,
		CurrentDriver,
		RealTime,
		OderMeter,
		PulseModulu,
		VehicleInfo,
		StateConfig,
		UniqCode,
		SpeedRecord,
		PositionRecord,
		AccidentSuspectPoint = 0x10,
		OverTimeDriving,
		DriverInfo,
		OutPowered,
		ParameterModify,
		SpeedStateLog
	};

	VTDRRecord();
	virtual ~VTDRRecord();
	unsigned char GetDataCode()
	{
		return (unsigned char) cDataCode;
	}
	;

	virtual int Read(const char* buf)=0;
	virtual string& Write(string& buf)=0;
	virtual string& Dump(string& buf)
	{
		stringstream stream(buf);
		stream << "DataCode:" << cDataCode;
		buf = stream.str();
		return buf;
	}
	;
	static string BCD2ASCII(string& strBCD);
	static unsigned int BCD2INT(const char* bcd, int size);
	static unsigned int BCD2INT(unsigned char bcd);
	static char INT2BCDchar(int n);
	static time_t ToSystime(VTDRTime& t);
	static VTDRTime& ToBCDTime(time_t t, VTDRTime& tm);
	static string Time2String(time_t t);
	static int utf8togb2312(const char *sourcebuf, size_t sourcelen,
			char *destbuf, size_t destlen);
	static int gb2312toutf8(const char *sourcebuf, size_t sourcelen,
			char *destbuf, size_t destlen);
	string UTF8ToGB2312(const string& str) const;
	string GB2312ToUTF8(const string& str) const;

	static float TenThoundthMinuteToMinute(int n)
	{
		return n / 10000.0;
	}
	;
	static int MinuteToTenThound(float n)
	{
		return (int) (n * 10000);
	}
	;
	template<class V> void assign(string& t, V v, int n)
	{
		t.assign((const char*) v, n);
	}
	;
	template<class V> void set(V v, string& t, int n)
	{
		memcpy(v, t.data(), min((int) t.size(), n));
	}
	;
protected:

	typedef struct _Position
	{
		int longititude;
		int latitude;
		short altitude;
	}__attribute__ ((packed)) Position;

	bool validPosition(const int pos)
	{
		return (ntohl(pos) != 0x7FFFFFFF) && (ntohl(pos) != 0xFFFFFFFF);
	}

	void readPosition(Position& pos, float& Long, float& Lat, int& Alt)
	{

		Long = validPosition(pos.longititude) ? ntohl(pos.longititude)
				/ 10000.0 :
				nanf("NAN");
		Lat = validPosition(pos.latitude) ? ntohl(pos.latitude) / 10000.0 :
				nanf("NAN");
		Alt = ntohs(pos.altitude);

	}
	;
	void writePosition(Position& pos, float Long, float Lat, int Alt)
	{
		pos.longititude = htonl((int) (Long * 10000.0));
		pos.latitude = htonl((int) (Lat * 10000.0));
		pos.altitude = htons(Alt);
	}
	;
	DataCode cDataCode;
};

#endif /* VTDRRECORD_H_ */
