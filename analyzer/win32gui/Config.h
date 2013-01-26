/*
 * Config.h
 *
 *  Created on: 2010-1-9
 *      Author: mxx
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define CONF_FILENAME "config.ini"

#include "stdafx.h"

class Config
{
public:
	bool ReadKey(LPCTSTR szKey, LPCTSTR szVal, int nLen);
	Config(const char* szFile);
	virtual ~Config();

	double GetMF_A(void);
	double GetMF_B(void);
	double GetMF_C(void);
	double GetMF_D(void);
	double GetMF_a(void);
	double GetMF_b(void);
	double GetMF_c(void);
	double GetMF_d(void);
protected:
	CString strFileName;
};



#endif /* CONFIG_H_ */
