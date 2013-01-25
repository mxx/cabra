/*
 * TraceLog.h
 *
 *  Created on: 2011-1-11
 *      Author: mxx
 */
////////////////////////////////////////////////////////
// Defind the common interface for trace and log
// To use trace, must use TraceLogInit() to setup
// message title, IP, and port
// then can use TRACE() to output
// TRACE() is the same format as printf()
// Example:
// ...
// TraceLogInit("TEST","127.0.0.1",1234); //output to localhost:1234
// SETTRACELEVEL(1);       //Only level higher equal than 1 is out
//...
//
// TRACE("This is a test message"); //output level 1 message
//				      // It's the always output message
// ...
// -------------------------------------------------------
// the output just like:
// 2010-01-06 14:31:58 TEST This is a test message
//
//
// Also can use the class methods to form your own message
// a global variable gTraceLog for class TraceLog exist already
//
// /////////////////////////////////////////////////////

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <string>

using namespace std;

struct Location
{
	const char* szFile;
	const char* szFunc;
	const char* szLine;
};

#define LP_NONE           0
#define LP_TRACE          1
#define LP_DEBUG          2
#define LP_INFO           3
#define LP_NOTICE         4
#define LP_WARNING	 	  5
#define LP_ERROR          6
#define LP_CRITICAL       7
#define LP_ALERT          8
#define LP_EMERGENCY      9
#define LP_UNINITIALIZED  -1

class TraceLog
{
public:

	TraceLog(void);
	TraceLog(const char* szIP,int nPort);
	TraceLog(const char* szT,const char* szIP,int nPort);
	~TraceLog();

	void Trace(int nLevel, const char* szFile, const char* szFunc,int nLine, const char* szFmt,...);
	void Dump(int nLev, const char* szFile, const char* szFunc ,int nLine,const char* buf,int len);
	static void Init(const char* Title,const char* szIP, int nPort);

	static inline void SetLocalOut(bool bEnable) {bLocal = bEnable;};
	static void SetTraceLevel(int n) { nLevel = n%10;};
	void EnableSysLog(bool enable){ bSysLog = enable;};
protected:
	static void SetTitle(char* szBuf,int nSize);		 //send out Date,Time and Title message
	void SendOut(const char* szBuf);
	bool bSysLog;
	static int nLogPort;
	static struct sockaddr_in sin;
	static int sct;
	static string strTitle;
	static char szLogIP[50];

	static const string priorityNames[];
	static int nLevel;
	static bool bLocal;
	static int nCounter;
};



#define TraceLogInit(x,y,z)	gTraceLog.Init(x,y,z)
#ifdef _TRACE_LOG_
extern TraceLog gTraceLog;
#define TRACE(args...) 		gTraceLog.Trace(LP_TRACE,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define DEBUG(args...) 		gTraceLog.Trace(LP_DEBUG,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define INFO(args...) 		gTraceLog.Trace(LP_INFO,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define NOTICE(args...) 	gTraceLog.Trace(LP_NOTICE,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define WARNING(args...)	gTraceLog.Trace(LP_WARNING,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define ERROR(args...) 		gTraceLog.Trace(LP_ERROR,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define CRITICAL(args...)	gTraceLog.Trace(LP_CRITICAL,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define ALERT(args...) 		gTraceLog.Trace(LP_ALERT,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define EMERGENCY(args...) 	gTraceLog.Trace(LP_EMERGENCY,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define DUMP(buf,len)		gTraceLog.Dump(LP_TRACE,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,buf,len)
#define SETLOCALOUT(x)		gTraceLog.SetLocalOut(x)
#define SETTRACELEVEL(x)	gTraceLog.SetTraceLevel(x)
#else
#define TRACE(args...)
#define DEBUG(args...)
#define INFO(args...)
#define NOTICE(args...)
#define WARNING(args...)
#define ERROR(args...)
#define CRITICAL(args...)
#define ALERT(args...)
#define EMERGENCY(args...)
#define DUMP(buf,len)
#define SETLOCALOUT(x)
#define SETTRACELEVEL(x)
#endif
//TODO: make it thread safe
#define ERRTRACE()	{gTraceLog.Trace(LP_ERROR,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,strerror(errno));}


#endif /* DEBUGLOG_H_ */
