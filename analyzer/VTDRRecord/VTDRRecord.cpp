/*
 * VTDRRecord.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mxx
 */

#include "VTDRRecord.h"
#ifdef _WIN32
#else
#include <iconv.h>
#endif

VTDRRecord::VTDRRecord() :
		cDataCode(Version)
{

}

VTDRRecord::~VTDRRecord()
{
	// TODO Auto-generated destructor stub
}

string VTDRRecord::BCD2ASCII(string& strBCD)
{
	string str;
	for (int i = 0; i < strBCD.size(); i++)
	{
		str += ((strBCD[i] & 0xF0) >> 4) + 0x30;
		str += ((strBCD[i] & 0x0F)) + 0x30;
	}
	return str;
}

unsigned int VTDRRecord::BCD2INT(const char* bcd, int size)
{
	unsigned int v = 0;
	for (int i = 0; i < size; i++)
	{
		char c = bcd[i];
		v = v * 100;
		v += ((c >> 4) & 0x0F) * 10 + (c & 0x0f);
	}
	return v;
}

unsigned int VTDRRecord::BCD2INT(unsigned char c)
{
	unsigned int v = 0;
	v += (((c >> 4) & 0x0F) * 10) + (c & 0x0f);
	return v;
}

char VTDRRecord::INT2BCDchar(int n)
{
	return ((((n % 100) / 10) & 0x0F) << 4) + (n % 100) % 10;
}

time_t VTDRRecord::ToSystime(VTDRTime& t)
{
	struct tm tmVal =
	{ 0 };
	tmVal.tm_year = BCD2INT(t.bcdYear) + 2000 - 1900;
	tmVal.tm_mon = BCD2INT(t.bcdMonth) - 1;
	tmVal.tm_mday = BCD2INT(t.bcdDay);
	tmVal.tm_hour = BCD2INT(t.bcdHour);
	tmVal.tm_min = BCD2INT(t.bcdMinute);
	tmVal.tm_sec = BCD2INT(t.bcdSecond);
	return mktime(&tmVal);
}

VTDRTime& VTDRRecord::ToBCDTime(time_t t, VTDRTime& tmBCD)
{
	struct tm tmVal;
#ifdef _WIN32
	tmVal = *localtime(&t);
#else
	localtime_r(&t, &tmVal);
#endif
	tmBCD.bcdYear = INT2BCDchar(tmVal.tm_year + 1900 - 2000);
	tmBCD.bcdMonth = INT2BCDchar(tmVal.tm_mon + 1);
	tmBCD.bcdDay = INT2BCDchar(tmVal.tm_mday);
	tmBCD.bcdHour = INT2BCDchar(tmVal.tm_hour);
	tmBCD.bcdMinute = INT2BCDchar(tmVal.tm_min);
	tmBCD.bcdSecond = INT2BCDchar(tmVal.tm_sec);

	return tmBCD;
}

string VTDRRecord::Time2String(time_t t)
{
	struct tm tmVal;
#ifdef _WIN32
	tmVal = *localtime(&t);
#else
	localtime_r(&t, &tmVal);
#endif
	char szTime[64] =
	{ 0 };
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", tmVal.tm_year + 1900,
			tmVal.tm_mon + 1, tmVal.tm_mday, tmVal.tm_hour, tmVal.tm_min,
			tmVal.tm_sec);
	return string(szTime);
}


int VTDRRecord::utf8togb2312(const char *sourcebuf, size_t sourcelen,
		char *destbuf, size_t destlen)
{
	int rt = 0;
#ifndef _WIN32
	iconv_t cd;
	
	if ((cd = iconv_open("gb2312", "utf-8")) == 0)
		return -1;
	memset(destbuf, 0, destlen);
	char **source = (char**) &sourcebuf;
	char **dest = &destbuf;

	if ((size_t) -1 == iconv(cd, source, &sourcelen, dest, &destlen))
		rt = -1;
	iconv_close(cd);
#else
	string strUTF(sourcebuf,sourcelen);
	UTF8ToGB2312(strUTF);
	strncpy(destbuf,strUTF.c_str(),destlen);
#endif
	return rt;

}

int VTDRRecord::gb2312toutf8(const char *sourcebuf, size_t sourcelen,
		char *destbuf, size_t destlen)
{
	int rt = 0;
#ifndef _WIN32
	iconv_t cd;
	
	if ((cd = iconv_open("utf-8", "gb2312")) == 0)
		return -1;
	memset(destbuf, 0, destlen);
	char **source = (char**) &sourcebuf;
	char **dest = &destbuf;

	if ((size_t) -1 == iconv(cd, source, &sourcelen, dest, &destlen))
		rt = -1;
	iconv_close(cd);
#else
	string strGB(sourcebuf,sourcelen);
	GB2312ToUTF8(strGB);
	strncpy(destbuf,strGB.c_str(),destlen);

#endif
	return rt;

}

string VTDRRecord::UTF8ToGB2312(const string& str) 
{
#ifndef _WIN32
	char* ptrBuf = new char[str.size() * 2];
	utf8togb2312(str.c_str(), str.size(), ptrBuf, str.size() * 2);
	string strR(ptrBuf);
	if (ptrBuf)
		delete ptrBuf;

#else
    int len=MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL,0);
    unsigned short * wszGBK = new unsigned short[len+1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK=new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte (CP_ACP, 0, wszGBK, -1, szGBK, len, NULL,NULL);

    string strR(szGBK);
    delete[] szGBK;
    delete[] wszGBK;
#endif
    return strR;
}

string VTDRRecord::GB2312ToUTF8(const string& str) 
{
#ifndef _WIN32
	char* ptrBuf = new char[str.size() * 2];
	gb2312toutf8(str.c_str(), str.size(), ptrBuf, str.size() * 2);
	string strR(ptrBuf);
	if (ptrBuf)
		delete ptrBuf;

#else
    int len=MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL,0);
    unsigned short * wszUtf8 = new unsigned short[len+1];
    memset(wszUtf8, 0, len * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszUtf8, len);

    len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
    char *szUtf8=new char[len + 1];
    memset(szUtf8, 0, len + 1);
    WideCharToMultiByte (CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL,NULL);

    string strR(szUtf8);
    delete[] szUtf8;
    delete[] wszUtf8;
#endif
    return strR;
}
