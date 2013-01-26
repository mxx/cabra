#include "USBDataFilev2012.h"
#include <stdio.h>
#include <string>
#include "TraceLog.h"
#include "VTDRRealTime.h"
using namespace std;
int main(int argc,const char** argv)
{
	SETTRACELEVEL(0);
	TRACE("start");
	USBDataFilev2012 file;
	string fn,fn2 ;
	fn = argv[1];
	VTDRRealTime r;
	int bcd = 0x12345678;
	TRACE("%d",r.BCD2INT((const char*)&bcd,4));
	string::size_type pos=fn.rfind("/");
	if (pos != string::npos)
	{
		fn2 = fn.substr(pos+1);
	}
	printf("Datafile %s\n",fn.c_str());
	printf("parse file name %s %s\n",fn2.c_str(),file.ParseFileName(fn2.c_str())?"OK":"Fail");
	try
	{
		file.ReadFromFile(fn.c_str());
	}
	catch(USBDataFileException &e)
	{
		printf("%s\n",e.strType.c_str());
	}
}
