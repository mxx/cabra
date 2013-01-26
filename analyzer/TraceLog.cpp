/*
 * DebugLog.cpp
 *
 *  Created on: 2011-1-11
 *      Author: mxx
 */

#include "TraceLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <syslog.h>
#define MAXBUFSIZE 1024

TraceLog gTraceLog;

const string TraceLog::priorityNames[] =
	{ "Zero Priority", "TRACE", "DEBUG", "INFO", "NOTICE", "WARNING", "ERROR",
			"CRITICAL ERROR", "ALERT", "EMERGENCY" };

int TraceLog::sct = -1;
int TraceLog::nLevel = 3;
bool TraceLog::bLocal = true;
int TraceLog::nCounter = 0;
int TraceLog::nLogPort = 0;
struct sockaddr_in TraceLog::sin = {0};
char TraceLog::szLogIP[50]={0};
string TraceLog::strTitle = "";

TraceLog::TraceLog()
{
	bSysLog = true;
}

TraceLog::~TraceLog()
{
	if (sct > 0)
		shutdown(sct, SHUT_RD);
}

TraceLog::TraceLog(const char* szIP, int nPort)
{
	sct = -1;
	nLevel = 0;
	bLocal = true;
	nCounter = 0;
	bSysLog = false;
	Init("", szIP, nPort);
}

TraceLog::TraceLog(const char* szTitle, const char* szIP, int nPort)
{
	sct = -1;
	nLevel = 0;
	bLocal = true;
	nCounter = 0;
	bSysLog = false;
	Init(szTitle, szIP, nPort);
}

void  TraceLog::Init(const char* Title, const char* szIP, int nPort)
{
	nLogPort = nPort;
	strncpy(szLogIP, szIP, 49);

	strTitle = Title;
	if (sct != -1)
		close( sct);
	sct = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket < 0)
	{
		perror("TraceLog::Init on socket");
	}
	memset(&sin, 0x0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(nLogPort);
	sin.sin_addr.s_addr = inet_addr(szLogIP);

	int so_broadcast=1;
	if (setsockopt(sct,SOL_SOCKET,SO_BROADCAST,
			&so_broadcast,sizeof(so_broadcast))==-1)
	{
		perror("setsockopt");
		return;
	}
}

void TraceLog::SetTitle(char* Buf, int nSize)
{
	struct tm Time =
	{ 0 };
	struct timeval tv =
	{ 0 };
	struct timezone tz =
	{ 0 };

	gettimeofday(&tv, &tz);

	localtime_r(&tv.tv_sec, &Time);
	if (bLocal)
		snprintf(Buf, nSize, "\n%02d-%02d %02d:%02d:%02d.%03d %s ",
					Time.tm_mon + 1, Time.tm_mday, Time.tm_hour, Time.tm_min,
					Time.tm_sec, (int)tv.tv_usec / 1000, strTitle.c_str());
	else
		snprintf(Buf, nSize, "\n%02d-%02d %02d:%02d:%02d.%03d %s %04d ",
			Time.tm_mon + 1, Time.tm_mday, Time.tm_hour, Time.tm_min,
			Time.tm_sec, (int)tv.tv_usec / 1000, strTitle.c_str(), nCounter);
	nCounter = (nCounter > 9998) ? 0 : nCounter + 1;
}

void TraceLog::SendOut(const char* szBuf)
{
	register int n = 0;
	if (bLocal)
	{
		if (puts(szBuf + 1) == EOF || fflush(stdout))
		{
			perror("TraceLog::SendOut:puts");
			clearerr(stdout);
		}
	}
	if (sct < 0 && !bLocal)
	{
		if (fputs(szBuf, stderr) == EOF || fflush(stderr))
		{
			perror("TraceLog::SendOut:fputs");
			clearerr(stderr);
		}
	}
	else if (sct>0)
	{
		n = sendto(sct, szBuf, strlen(szBuf), MSG_DONTWAIT,
				(struct sockaddr *) & sin, sizeof(sin));
		if (-1 == n)
		{
			perror("\nTraceLog::SendOut:sendto");
			fputs(szBuf + 1, stderr);
			fputs("\n", stderr);
		}
	}
}

void TraceLog::Dump(int nLev, const char* szFile, const char* szFunc ,int nLine,const char* buf,int len)
{
	if (nLev <= nLevel)
		return;
	int n;
	int size = len*3+128;
	char* pBuf = NULL;
	char szBuf[80] = { 0 };


	if ((pBuf = (char*) malloc(size)) == NULL)
	{
		pBuf = szBuf;
		size = 80;
	}

	SetTitle(pBuf, size);
	int used = strlen(pBuf);
	if (used < size)
	{
		snprintf(pBuf+used, size-used, "%s %s:%u[%s]\t",priorityNames[nLev].c_str(),szFile,nLine,szFunc);
		used = strlen(pBuf);
	}
		/* Try to print in the allocated space. */
	if (used < size)
	{
		for (int i = 0; i < len; i++)
		{
			if (used >= size) break;
			n = snprintf(pBuf + used, size - used, "%02hhX ", buf[i]);
			if (n > 0)
				used += n;
			else
				break;
		}
	}

	/* If that worked, return the string. */
	SendOut(pBuf);
	if (80 != size)
		free(pBuf);
	return;
}

void TraceLog::Trace(int nLev, const char* szFile, const char* szFunc ,int nLine, const char* szFmt, ...)
{
	if (nLev < nLevel)
		return;
	va_list ap;
	int n;
	int size = 256;
	char* pBuf = NULL;
	char szBuf[80] =
	{ 0 };
	if ((pBuf = (char*) malloc(size)) == NULL)
	{
		pBuf = szBuf;
		size = 80;
	}

	SetTitle(pBuf, size);
	int used = strlen(pBuf);
	int nSysLogStart;
	if (used < size)
	{
		snprintf(pBuf+used, size-used, "%s %s:%u[%s]\t",priorityNames[nLev].c_str(),szFile,nLine,szFunc);
		used = strlen(pBuf);
	}
	nSysLogStart = used;
	while (1)
	{
		/* Try to print in the allocated space. */
		va_start(ap, szFmt);
		n = vsnprintf(pBuf + used, size - used, szFmt, ap);
		va_end(ap);

		/* If that worked, return the string. */
		if (80 == size || (n > -1 && n < size - used) || size == MAXBUFSIZE)
		{
			SendOut(pBuf);
			nLev= 8 - nLev;
			if (nLev < 0) nLev = 0;
			if (bSysLog) syslog(nLev,"%s\n",pBuf+nSysLogStart);
			if (80 != size)
				free(pBuf);
			return;
		}
		/* Else try again with more space. */
		char* ptmp = pBuf;
		if (n > -1) /* glibc 2.1 */
			size = min(used + n + 1, MAXBUFSIZE); /* precisely what is needed */
		else
			/* glibc 2.0 */
			size = min(size + 80, MAXBUFSIZE); /* each time increase 80 bytes */
		if ((pBuf = (char*) realloc(pBuf, size)) == NULL)
		{
			//if failed extending, just send already ones
			SendOut(ptmp);
			nLev= 8 - nLev;
			if (nLev < 0) nLev = 0;
			if (bSysLog) syslog(nLev,"%s",ptmp+nSysLogStart);
			free(ptmp);
			return;
		}
	}
}

