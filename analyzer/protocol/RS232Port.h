/*
 * RS232Port.h
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */

#ifndef RS232PORT_H_
#define RS232PORT_H_

#include <string>
using namespace std;

class RS232Port
{
public:
	RS232Port();
	virtual ~RS232Port();
	int Open(const char* szDev);
	int Open();
	int Read(char* buf,int len);
	int Write(const char* buf,int len);
	void SetCom();
	void Close();
	void Flush();
protected:
	int handle;
	int timeout;
	string strDevName;
};

#endif /* RS232PORT_H_ */
