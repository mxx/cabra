/*
 * RS232Port.cpp
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <termios.h>
#include "RS232Port.h"
#include "DebugLog.h"

RS232Port::RS232Port()
{
	handle = -1;
	timeout = 20000;
}

RS232Port::~RS232Port()
{
	if (handle > 0)
		Close();
}
int RS232Port::Open(const char *szDev)
{
	if (strDevName != szDev)
		strDevName = szDev;

	return Open();
}

int RS232Port::Open()
{
	Close();
	INFO("%s",strDevName.c_str());

	handle = open(strDevName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (handle < 0)
	{
		ERRTRACE();
		return -1;
	}
	SetCom();
	return 0;
}

void RS232Port::SetCom()
{
	if (handle < 0)
		return;

	struct termios newtio;
	tcflush(handle, TCIOFLUSH);

	tcgetattr(handle, &newtio); /* save current port settings */
	/* set new port settings for canonical input processing */
	newtio.c_cflag = 0;
	//newtio.c_cflag &= ~(CSIZE | PARENB| PARODD );
	newtio.c_cflag = B9600 | CS8 |  CLOCAL  | CREAD |  PARODD | PARENB;

	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);

	newtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR
			| ICRNL | IXON);

	//newtio.c_oflag &= ~OPOST  ;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_cc[VMIN] = 0;
	newtio.c_cc[VTIME] = 0;

	int n = tcsetattr(handle, TCSANOW, &newtio);
	if (n < 0)
		ERRTRACE();
}



int RS232Port::Read(char *buf, int len)
{
	if (len == 0)
		return 0;
	if (handle == -1)
		Open();
	int try_again = 2;
	int n;
	do
	{
		n = read(handle, buf, len);

		if (n > 0)
			return n;
		if (n == -1)
		{
			ERRTRACE();
			Close();
			return 0;
		}
		if (try_again--)
		{
			TRACE("wait");
			usleep(timeout);
			continue;
		}
	} while (try_again);
	if (n==0) TRACE("no byte read");
	return n;
}



int RS232Port::Write(const char *buf, int len)
{
	if (len == 0)
		return 0;
	if (handle == -1)
		Open(strDevName.c_str());

	DUMP(buf,len);

	int n = write(handle, buf, len);
	tcdrain(handle);
	if (n > 0)
		return n;
	if (n == -1)
	{
		ERRTRACE();
		Close();
	}
	return 0;
}

void RS232Port::Flush()
{
	if (handle!=-1) tcflush(handle, TCIFLUSH);
}

void RS232Port::Close()
{
	if (handle > 0)
	{
		tcflush(handle, TCIOFLUSH);
		close(handle);
		handle = -1;
	}
}






